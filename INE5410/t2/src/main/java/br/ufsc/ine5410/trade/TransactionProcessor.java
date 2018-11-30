package br.ufsc.ine5410.trade;

import br.ufsc.ine5410.bank.Account;

import java.util.concurrent.locks.ReentrantReadWriteLock;
import javax.annotation.Nonnull;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.TimeUnit;

public class TransactionProcessor implements AutoCloseable {
    private ReentrantReadWriteLock lock = new ReentrantReadWriteLock();
    private boolean closed = false;
    private ExecutorService executor = Executors.newCachedThreadPool();

    public void process(@Nonnull final OrderBook orderBook, @Nonnull final Transaction trans) {
        Order buy = trans.getBuy(), sell = trans.getSell();
        // Account fromAcc = buy.getBroker().getAccount(buy.getInvestor());
        // Account toAcc = sell.getBroker().getAccount(sell.getInvestor());

        lock.readLock().lock();
        // Return if processor was closed
        try {
            if (closed) {
                buy.notifyCancellation();
                sell.notifyCancellation();
                return;
            }

            Worker worker = new Worker(lock, orderBook, trans);
            try {
                executor.execute(worker);

            } catch (RejectedExecutionException e) {
                lock.readLock().unlock();
                e.printStackTrace();
            }
        } finally {
            lock.readLock().unlock();
        }
    }

    @Override
    public void close() {
        lock.readLock().lock();
        try {
            if (this.closed) {
                return;
            }
        } finally {
            lock.readLock().unlock();
        }
        lock.writeLock().lock();
        try {
            this.closed = true;

        } finally {
            lock.writeLock().unlock();
        }

        executor.shutdown();
        try {
            executor.awaitTermination(Long.MAX_VALUE, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private class Worker implements Runnable {
        private ReentrantReadWriteLock lock;
        private OrderBook orderBook;
        private Transaction trans;

        public Worker(ReentrantReadWriteLock lock, OrderBook orderBook, Transaction trans) {
            this.lock = lock;
            this.orderBook = orderBook;
            this.trans = trans;
        }

        @Override
        public void run() {
            this.lock.readLock().lock();
            Order buy = this.trans.getBuy(), sell = this.trans.getSell();
            Account fromAcc = buy.getBroker().getAccount(buy.getInvestor());
            Account toAcc = sell.getBroker().getAccount(sell.getInvestor());

            try {
                // handle missing accounts
                if (fromAcc == null)
                    buy.notifyCancellation();
                if (toAcc == null)
                    sell.notifyCancellation();
                if (fromAcc == null || toAcc == null) {
                    // retry the order with account
                    if (fromAcc != null)
                        this.orderBook.post(buy);
                    if (toAcc != null)
                        this.orderBook.post(sell);
                    return;
                }

                // buyer defines price
                double price = buy.getPrice();
                assert price >= sell.getPrice();

                // Solving deadlock
                long id_from = fromAcc.getId();
                long id_to = toAcc.getId();
                if (id_from > id_to) {
                    fromAcc.getLock().lock();
                    toAcc.getLock().lock();
                } else {
                    toAcc.getLock().lock();
                    fromAcc.getLock().lock();
                }
                // transfer the money
                try {
                    boolean ok = fromAcc.withdraw(price);
                    if (!ok) {
                        orderBook.post(sell); // try later
                        buy.notifyCancellation(); // fail
                    } else {
                        try {
                            // throws if client does not have the stock
                            toAcc.removeStock(buy.getStock());

                            toAcc.deposit(price);
                            fromAcc.addStock(buy.getStock());
                            buy.notifyExecution(); // done!
                            sell.notifyExecution(); // done!
                        } catch (NoSuchStockException e) {
                            fromAcc.deposit(price);
                            sell.notifyCancellation(); // fail
                            orderBook.post(buy); // try later
                        }
                    }
                } finally {
                    toAcc.getLock().unlock();
                    fromAcc.getLock().unlock();
                }

            } finally {
                this.lock.readLock().unlock();
            }
        }
    }
}
