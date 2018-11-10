
package org.example.ine5410;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.*;

public class MergeSortExecutor<T extends Comparable<T>> implements MergeSort<T> {
    @Nonnull
    @Override
    public ArrayList<T> sort(@Nonnull List<T> list) {
        //1. Crie um Cached ExecutorService
        // (Executors Single thread ou fixed thread pool) causarão starvation!
        //2. Submete uma tarefa incial ao executor
        //3. Essa tarefa inicial vai se subdividir em novas tarefas enviadas para
        //   o mesmo executor
        //4. Desligue o executor ao sair!

        // Ver o tamanho bom para serial e paralelo: Chamar o MergeSortSerial e dar return no método
        if (list.size() <= 1)
            return new ArrayList<>(list);

        /* ~~~~ O tipo do executor precisa ser Cached!!!! ~~~~ */
        ExecutorService executor = Executors.newCachedThreadPool();
        int mid = list.size() / 2;
        ArrayList<T> left = null;
        left = sort(list.subList(0, mid));

        // Errado porque future eh uma interface e nao pode ser instanciada
        //Future<ArrayList<T>> future = new Future<>();

        Future<ArrayList<T>> future = executor.submit(new WorkerCallable(left, executor));
        List<T> finalList = null;
        try {
        	finalList = future.get();
        } catch (InterruptedException | ExecutionException ex) {
        	ex.printStackTrace();
        }
        ArrayList<T> right = sort(list.subList(mid, list.size()));

        try {
            executor.shutdown();
            executor.awaitTermination(Long.MAX_VALUE, TimeUnit.SECONDS);
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        return MergeSortHelper.merge(finalList, right);
        //throw new UnsupportedOperationException("Me implemente!");
    }

    // Classe para pegar os retornos das threads
    private class WorkerCallable implements Callable <ArrayList<T>> {
        private List<T> list;
        private ExecutorService executor;

        public WorkerCallable(List<T> list, ExecutorService executor) {
            this.list = list;
            this.executor = executor;
        }

        @Override
        public ArrayList<T> call() {
            int mid = this.list.size() / 2;
            List<T> left = this.list.subList(0, mid);
            this.executor.submit(new WorkerCallable(left, executor));
            ArrayList<T> right = new WorkerCallable(this.list.subList(mid, this.list.size()), executor).call();
            return MergeSortHelper.merge(left, right);
        }
    }
}
