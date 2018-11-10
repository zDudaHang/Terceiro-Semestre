package br.ufsc.atividade9;

import javax.annotation.Nonnull;
import java.util.concurrent.*;

public class Tribunal implements AutoCloseable {
    protected final ExecutorService executor;
    protected int nJuizes;
    protected int tamFila;
    
    public Tribunal(int  nJuizes, int tamFila) {
    	this.executor = new ThreadPoolExecutor(nJuizes, nJuizes, 0, TimeUnit.MILLISECONDS, new ArrayBlockingQueue<Runnable>(tamFila), new ThreadPoolExecutor.AbortPolicy());
        this.nJuizes = nJuizes;
        this.tamFila = tamFila;
    }

    public boolean julgar(@Nonnull final Processo processo)  throws TribunalSobrecarregadoException {
    	Future<Boolean> return_guilty;
    	//Future<Boolean>[tamFila] return_guiltys = new Future[tamFila];
    	try {
    		return_guilty = executor.submit(new WorkerCallable(processo));
    	} catch (RejectedExecutionException ex) {
    		// O AbortPolicy sempre ira gerar uma excecao do tipo RejectedExecutionException quando:
    		// 1) Insercao na fila cheia
    		// 2) Submit depois de um shutdown
    		throw new TribunalSobrecarregadoException();
    	}
        try {
			return return_guilty.get();
		} catch (InterruptedException | ExecutionException e) {
			e.printStackTrace();
		}
		return false;
    }

    protected boolean checkGuilty(Processo processo) {
        try {
            Thread.sleep((long) (50 + 50*Math.random()));
        } catch (InterruptedException ignored) {}
        return processo.getId() % 7 == 0;
    }

    @Override
    public void close() throws Exception {
    	try {
            executor.shutdown();
            executor.awaitTermination(Long.MAX_VALUE, TimeUnit.SECONDS);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }
    }
    
    private class WorkerCallable implements Callable <Boolean> {
        private Processo processo;

        public WorkerCallable(Processo processo) {
            this.processo = processo;
        }

        @Override
        public Boolean call() {
            return checkGuilty(processo);
        }
    }
}
