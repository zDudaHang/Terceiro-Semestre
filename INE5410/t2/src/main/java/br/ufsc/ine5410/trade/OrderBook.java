package br.ufsc.ine5410.trade;

import java.util.concurrent.atomic.AtomicLong;
import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.*;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

import static br.ufsc.ine5410.trade.Order.Type.*;

public class OrderBook implements AutoCloseable {
    private final @Nonnull String stockCode;
    private final @Nonnull TransactionProcessor transactionProcessor;
    private final @Nonnull PriorityBlockingQueue<Order> sellOrders, buyOrders;
    private boolean closed = false;
    private ReentrantLock order_lock = new ReentrantLock();
    private Semaphore sem = new Semaphore(0);
    // Interns, avoiding reinstantiation
    private Order buy, sell;
    private Transaction trans;

    private AtomicLong timestamp = new AtomicLong(0);

    public OrderBook(@Nonnull String stockCode, @Nonnull TransactionProcessor transactionProcessor) {
        this.stockCode = stockCode;
        this.transactionProcessor = transactionProcessor;
        sellOrders = new PriorityBlockingQueue<>(100, new Comparator<Order>() {
            @Override
            public int compare(@Nonnull Order l, @Nonnull Order r) {
                if (l.getPrice() != r.getPrice())
                    return Double.compare(l.getPrice(), r.getPrice());
                else
                    return Long.compare(r.getTimestamp(), l.getTimestamp());
            }
        });
        buyOrders = new PriorityBlockingQueue<>(100, new Comparator<Order>() {
            @Override
            public int compare(@Nonnull Order l, @Nonnull Order r) {
                if (l.getPrice() != r.getPrice())
                    return Double.compare(r.getPrice(), l.getPrice());
                else
                    return Long.compare(r.getTimestamp(), l.getTimestamp());
            }
        });

        new Thread(new Runnable() {
            @Override
            public void run() {
                tryMatch();
            }
        }).start();
    }

    public void post(@Nonnull Order order) {
        if (!order.getStock().equals(stockCode)) {
            String msg = toString() + " cannot process orders for " + order.getStock();
            throw new IllegalArgumentException(msg);
        }
        if (closed) {
            order.notifyCancellation();
            return;
        }
        order.notifyQueued();

        order.setTimestamp(timestamp.incrementAndGet());

        (order.getType() == BUY ? buyOrders : sellOrders).add(order);
        sem.release();
    }

    private @Nullable Transaction getMatch() {
        buy = buyOrders.poll();
        sell = sellOrders.poll();

        if ((sell != null && buy != null) && (sell.getPrice() <= buy.getPrice())) {
            sell.notifyProcessing();
            buy.notifyProcessing();
            return new Transaction(sell, buy);
        } else {
            if (sell != null) {
                sellOrders.add(sell);
            }
            if (buy != null) {
                buyOrders.add(buy);
            }

            return null;
        }
    }

    private void tryMatch() {
        while (true) {
            try {
                sem.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            if (closed) {
                break;
            }

            order_lock.lock();
            try {
                while ((trans = getMatch()) != null) {
                    transactionProcessor.process(OrderBook.this, trans);
                }
            } finally {
                order_lock.unlock();
            }

        }
    }

    @Override
    public String toString() {
        return String.format("OrderBook(%s)", stockCode);
    }

    @Override
    public void close() {
        if (closed)
            return;
        closed = true;
        // any future post() call will be a no-op
        this.order_lock.lock();
        try {
            for (Order order : sellOrders)
                order.notifyCancellation();
            sellOrders.clear();
            for (Order order : buyOrders)
                order.notifyCancellation();
            buyOrders.clear();
        } finally {
            this.order_lock.unlock();
        }
    }
}
