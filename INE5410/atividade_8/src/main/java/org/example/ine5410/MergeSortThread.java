package org.example.ine5410;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.List;

public class MergeSortThread<T extends Comparable<T>> implements MergeSort<T> {
    @Nonnull
    @Override
    public ArrayList<T> sort(@Nonnull final List<T> list) {
        //1. Há duas sub-tarefas, execute-as em paralelo usando threads
        //  (Para pegar um retorno da thread filha faça ela escrever em um ArrayList)
        
        
        if (list.size() <= 1)
            return new ArrayList<>(list);

        int mid = list.size() / 2;
        ArrayList<T> left = null;
        /* ~~~~ Execute essa linha paralelamente! ~~~~ */
        //left = sort(list.subList(0, mid));
        WorkerThread myThread = new WorkerThread(list);
        myThread.start();

        ArrayList<T> right = sort(list.subList(mid, list.size()));
        try {
            myThread.join();
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        return MergeSortHelper.merge(left, right);
    }

    // Classe da Thread com o método run já implementado
    private class WorkerThread extends Thread {
        private List<T> list;
        private ArrayList<T> returnList;

        public WorkerThread(List<T> list) {
            this.list = list;
            this.returnList = null;
        }

        public void setReturnList(ArrayList<T> returnList) {
            this.returnList = returnList;
        }

        @Override
        public void run() {
            int mid = this.list.size() / 2;
            ArrayList<T> left = sort(this.list.subList(0, mid));
            WorkerThread worker = new WorkerThread(left);
            worker.start();
            ArrayList<T> right = sort(this.list.subList(mid, this.list.size()));
            try {
				worker.join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
            setReturnList(MergeSortHelper.merge(left, right));
        }
    }
}
