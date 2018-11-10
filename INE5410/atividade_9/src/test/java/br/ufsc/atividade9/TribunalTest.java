package br.ufsc.atividade9;

import org.apache.commons.lang3.time.StopWatch;
import org.junit.*;
import org.junit.experimental.runners.Enclosed;
import org.junit.runner.RunWith;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

import static br.ufsc.atividade9.Processo.Tipo.*;
import static java.lang.String.format;

@RunWith(Enclosed.class)
public class TribunalTest {
    public static class Sanity {
        private Tribunal tribunal;

        @After
        public void tearDown() throws Exception {
            final boolean wasRun[] = {false};
            tribunal.executor.submit(new Runnable() {
                @Override
                public void run() {
                    try {
                        Thread.sleep(300);
                    } catch (InterruptedException ignored) {}
                    wasRun[0] = true;
                }
            });
            tribunal.close();
            Assert.assertTrue("Tribunal.close() não esperou shutdown do executor terminar",
                              wasRun[0]);
            Assert.assertTrue(tribunal.executor.isShutdown());
            Assert.assertTrue(tribunal.executor.isTerminated());
        }

        @Test(timeout = 2000)
        public void testEnqueuedRequests() throws TribunalSobrecarregadoException {
            tribunal = new MockTribunal(1, 1, 0);
            Assert.assertTrue(tribunal.julgar(new Processo(7, HOMICIDIO)));
            Assert.assertFalse(tribunal.julgar(new Processo(5, LATROCINIO)));
        }
    }

    public static class FullQueue  {
        private static long duration = Long.MAX_VALUE;
        private static long totalDuration = Long.MAX_VALUE;
        private static int maxConcurrentJudgments = Integer.MAX_VALUE;
        private static Boolean culpado = true;
        private static List<Boolean> results = new ArrayList<>();


        @BeforeClass
        public static void setUp() throws ExecutionException, InterruptedException, TimeoutException {
            Executors.newSingleThreadExecutor().submit(new Callable<Void>() {
                @Override
                public Void call() throws Exception {
                    final MockTribunal tribunal = new MockTribunal(2, 4, 1000);
                    ExecutorService ex = Executors.newCachedThreadPool();
                    List<Future<Boolean>> futures = new ArrayList<>();
                    StopWatch outerSw = StopWatch.createStarted();
                    for (int i = 0; i < 8; i++) {
                        final int id =  i < 4 ? i*7 : 1+(i%7)*3;
                        futures.add(ex.submit(new Callable<Boolean>() {
                            @Override
                            public Boolean call() {
                                try {
                                    return tribunal.julgar(new Processo(id, FURTO));
                                } catch (TribunalSobrecarregadoException e) {
                                    return null;
                                }
                            }
                        }));
                    }
                    StopWatch sw = StopWatch.createStarted();
                    try {
                        culpado = tribunal.julgar(new Processo(7, FURTO));
                    } catch (TribunalSobrecarregadoException e) {
                        culpado = null;
                    }
                    duration = sw.getTime();

                    for (Future<Boolean> future : futures) results.add(future.get());
                    totalDuration = outerSw.getTime();
                    maxConcurrentJudgments = tribunal.maxConcurrentJudgments.get();

                    tribunal.close();
                    Assert.assertTrue(tribunal.executor.isShutdown());
                    Assert.assertTrue(tribunal.executor.isTerminated());

                    return null;
                }
            }).get(8*1000 + 2000, TimeUnit.MILLISECONDS);
        }

        @AfterClass
        public static void tearDown() {
            duration = Long.MAX_VALUE;
            totalDuration = Long.MAX_VALUE;
            maxConcurrentJudgments = Integer.MAX_VALUE;
            culpado = true;
            results.clear();
        }

        @Test
        public void testrejeitadoRapido() {
            Assert.assertTrue("Fila estava cheia e processo demorou muito para ser " +
                            "julgado. Deveria ter sido inocentado imediatamente, mas levou "
                            + duration + "ms.",
                    duration < 750);
        }

        @Test
        public void testJulgamentosEmParalelo() {
            Assert.assertTrue(totalDuration < 8*1000 + 900);
            Assert.assertTrue(totalDuration < (8/2)*1000 + 900);
            Assert.assertTrue(maxConcurrentJudgments <= 2);
        }

        @Test
        public void testResultados() {
            Assert.assertNull("Processo com culpado foi colocado com a fila cheia, " +
                    "mas o culpado não foi inocentado", culpado);
            for (int i = 0; i < 4; i++) {
                Assert.assertTrue(format("Processo i=%d inocentado indevidamente!", i),
                        results.get(i));
            }
            for (int i = 4; i < 8; i++) {
                // esses resultados devem ser false ou null
                //noinspection PointlessBooleanExpression,SimplifiableJUnitAssertion
                Assert.assertFalse(format("Processo i=%d culpado indevidamente!", i),
                        results.get(i) == Boolean.TRUE);
            }
        }
    }

    private static class MockTribunal extends Tribunal {
        private int waitMs;
        public final AtomicInteger maxConcurrentJudgments = new AtomicInteger(0);
        private final AtomicInteger concurrentJudgments = new AtomicInteger(0);

        public MockTribunal(int nJuizes, int tamFila, int waitMs) {
            super(nJuizes, tamFila);
            this.waitMs = waitMs;
        }

        @Override
        protected boolean checkGuilty(Processo processo) {
            concurrentJudgments.incrementAndGet();
            try {
                Thread.sleep(waitMs);
            } catch (InterruptedException ignored) {}
            int value = concurrentJudgments.decrementAndGet()+1;
            int currMax = maxConcurrentJudgments.get();
            while (value > currMax) {
                if (maxConcurrentJudgments.compareAndSet(currMax, value)) break;
                currMax = maxConcurrentJudgments.get();
            }
            return processo.getId() % 7 == 0;
        }
    }
}