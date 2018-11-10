package br.ufsc.atividade10;

import javax.annotation.Nonnull;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import static br.ufsc.atividade10.Piece.Type.*;

public class Buffer {
    private final int maxSize;
    private LinkedList<Piece> buffer;
    private int countX = 0;
    private int countO = 0;

    public Buffer() {
        this(10);
    }
    public Buffer(int maxSize) {
        this.maxSize = maxSize;
        this.buffer = new LinkedList<Piece>();
    }

    //! Uma thread por vez pode acessar esse método
    //! Exclusão Mútua garantida
    //! O : maxSize - 1
    //! X : maxSize - 2
    public synchronized void add(Piece piece) throws InterruptedException {
       	if (piece.getType() == X) {
       		//! O notifyAll() tira a thread do wait(), mas
       		//! nao quer dizer que a condicao foi satisfeita
       		//! por isso:
       		while (countX >= maxSize - 2 || buffer.size() >= maxSize) {
       			// piece = null;
        		wait();
        	}
    		assert countX < maxSize - 2 || buffer.size() < maxSize;
    		buffer.add(piece);
    		countX++;
    		notifyAll();
    	} else {
    		while (countO >= maxSize - 1 || buffer.size() >= maxSize) {
    			// piece = null;
        		wait();
        	}
        	assert countO <= maxSize - 1 || buffer.size() < maxSize;
        	buffer.add(piece);
        	countO++;
        	notifyAll();
    	}
    }

    //! Uma thread por vez pode acessar esse método
    //! Exclusão Mútua garantida
    //! Esperar até ter:
    //! 2 O e 1 X
    public synchronized void takeOXO(@Nonnull List<Piece> xList,
                                     @Nonnull List<Piece> oList) throws InterruptedException {
    	while (countX < 1 || countO < 2) {
    		wait();
    	}
    	assert countX >= 1 && countO >= 2;
    	Iterator<Piece> iterator = buffer.iterator();
    	while(iterator.hasNext() && xList.size() < 1 || oList.size() < 2) {
    		Piece current = iterator.next();
    		if (current.getType() == X && xList.size() < 1) {
    			xList.add(current);
    			countX--;
    			iterator.remove();
    			notifyAll();
    		} else {
    			if (current.getType() == O && oList.size() < 2) {
	    			oList.add(current);
	    			countO--;
	    			iterator.remove();
	    			notifyAll();
    			}
    		}
    	}
    }
}
