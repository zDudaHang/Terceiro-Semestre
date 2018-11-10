#include "cozinha.h"

extern void cozinha_init(int cozinheiros, int bocas, int frigideiras, int garcons, int tam_balcao) {
    // Inicialização dos semáforos
	sem_init(&s_bocas, 0, bocas);
	sem_init(&s_frigideiras, 0, frigideiras);
    sem_init(&s_balcao_garcons, 0, 0);
    sem_init(&s_balcao_cozinheiros, 0, tam_balcao);
    sem_init(&s_cozinheiros, 0, cozinheiros);

    // Tamanho do balcao
    balcao = tam_balcao;

    // Numero de cozinheiros para iteração
    num_cozinheiros = cozinheiros;

    // Numero de garcons para iteração
    num_garcons = garcons;

    // Indices dos produtores  e consumidor
    index_garcon = 0;
    index_cozinheiro = 0;

    // Pratos no balcao
    pratos_prontos = (prato_t**) malloc(sizeof(prato_t*) * tam_balcao);

    // Alocação de memória p/ garcons
    garcons_threads = (pthread_t*) malloc(sizeof(pthread_t) * garcons);

    // Inicializar mutexes
    pthread_mutex_init(&mutex_coleta_garcon, NULL);
    pthread_mutex_init(&mutex_entrega_cozinheiro, NULL);

    // Criar threads de garcon
    for (int i = 0; i < garcons; i++) {
        pthread_create(&garcons_threads[i], NULL, garcon_thread, NULL);
    }
}

extern void cozinha_destroy() {
    // Espera cozinheiros terminarem suas tarefas e demite eles
    for (int i = 0; i < num_cozinheiros; i++) {
        sem_wait(&s_cozinheiros);
    }

    // Finaliza os garcon com pratos nulos
    prato_t* prato_nulo;
    for (int i  = 0; i < num_garcons; i++) {
        // Criando um prato nulo da demissão
        prato_nulo = (prato_t*) malloc(sizeof(prato_t));
        prato_nulo->pedido.prato = PEDIDO_NULL;

        // Mexendo no balcão, sem mutex por que os cozinheiros foram demitidos
        sem_wait(&s_balcao_cozinheiros);
        pthread_mutex_lock(&mutex_entrega_cozinheiro);
        pratos_prontos[index_cozinheiro] = prato_nulo;
        index_cozinheiro = (index_cozinheiro + 1) % balcao;
        pthread_mutex_unlock(&mutex_entrega_cozinheiro);
        sem_post(&s_balcao_garcons);
    }

    for (int i  = 0; i < num_garcons; i++) {
        pthread_join(garcons_threads[i], NULL);
    }

    // Destruir os semáforos
	sem_destroy(&s_bocas);
	sem_destroy(&s_frigideiras);
    sem_destroy(&s_balcao_garcons);
    sem_destroy(&s_balcao_cozinheiros);
    sem_destroy(&s_cozinheiros);

    // Destruir mutexes
    pthread_mutex_destroy(&mutex_coleta_garcon);
    pthread_mutex_destroy(&mutex_entrega_cozinheiro);

    // Desaloca vetores
    free(garcons_threads);
    free(pratos_prontos);


    /*   __  __                       _                                                 _                   
        |  \/  |   ___      o O O  __| |   __ _      o O O  _  _    _ __      o O O  __| |    ___      ___  
        | |\/| |  / -_)    o      / _` |  / _` |    o      | +| |  | '  \    o      / _` |   / -_)    |_ /  
        |_|__|_|  \___|   TS__[O] \__,_|  \__,_|   TS__[O]  \_,_|  |_|_|_|  TS__[O] \__,_|   \___|   _/__|  
        _|"""""|_|"""""| {======|_|"""""|_|"""""| {======|_|"""""|_|"""""| {======|_|"""""|_|"""""|_|"""""| 
        "`-0-0-'"`-0-0-'./o--000'"`-0-0-'"`-0-0-'./o--000'"`-0-0-'"`-0-0-'./o--000'"`-0-0-'"`-0-0-'"`-0-0-' 

                                              .--,--.
                                              `.  ,.'
                                               |___|
                                               :o o:   O    Dom Mario e seu falecido sobrinho 
                                              _`~^~'_  |       aprovaram essa simulação
                                            /'   ^   `\=)
                                          .'  _______ '~|
                                          `(<=|     |= /'
                                              |     |
                                              |_____|
                                       ~~~~~~~ ===== ~~~~~~~~
    */
}

extern void processar_pedido(pedido_t pedido) {
    // Espera cozinheiro vago
	sem_wait(&s_cozinheiros);

    // Alocando memória para passar o pedido para a thread do cozinheiro
    pedido_t* pedido_ptr = (pedido_t*) malloc(sizeof(pedido_t));
    *pedido_ptr = pedido;

    pthread_t thread;
    pthread_create(&thread,
                    NULL,
                    cozinheiro_thread,
                    (void*)pedido_ptr);
}

void* fervendo_agua_thread(void* arg) {
	agua_t* agua = (agua_t*) arg;

    // Espera por uma boca de fogao livre
    sem_wait(&s_bocas);

    // Ferve a agua
	ferver_agua(agua);

    // Libera  a boca do fogão	
    sem_post(&s_bocas);

    return NULL;
}

void* dourando_bacon_thread(void* arg) {
    bacon_t* bacon = (bacon_t*) arg;

    // Espera frigideira
    sem_wait(&s_frigideiras);

    // Espera por uma boca de fogao livre
    sem_wait(&s_bocas);

    // Doura o bacon
    dourar_bacon(bacon);

    // Libera frigideira
    sem_post(&s_frigideiras);

    // Libera  a boca do fogão	
    sem_post(&s_bocas);

    return NULL;
}

void* esquentando_molho_thread(void* arg) {
    molho_t* molho = (molho_t*) arg;

    // Espera por uma boca de fogao livre
    sem_wait(&s_bocas);

    // Esquenta o molho
    esquentar_molho(molho);

    // Libera  a boca do fogão	
    sem_post(&s_bocas);
    
    return NULL;
}

void prepara_carne(pedido_t pedido) {
    prato_t* prato = create_prato(pedido);
    carne_t* carne = create_carne();

    cortar_carne(carne);
    temperar_carne(carne);

    // Espera por frigideira disponível
    sem_wait(&s_frigideiras);

    // Espera boca no fogão
    sem_wait(&s_bocas);


    grelhar_carne(carne);

    // Libera boca do fogão
    sem_post(&s_bocas);

    // Libera frigideiras
    sem_post(&s_frigideiras);


    empratar_carne(carne, prato);

    // Espera espaço no balcão
    sem_wait(&s_balcao_cozinheiros);

    // Mexendo no balcão
    pthread_mutex_lock(&mutex_entrega_cozinheiro);
    pratos_prontos[index_cozinheiro] = prato;
    index_cozinheiro = (index_cozinheiro + 1) % balcao;
    pthread_mutex_unlock(&mutex_entrega_cozinheiro);

    // Notifica pedido
    notificar_prato_no_balcao(prato);

    // Avisa garçons
    sem_post(&s_balcao_garcons);
}

void prepara_sopa(pedido_t pedido) {
    prato_t* prato = create_prato(pedido);
    agua_t* agua = create_agua();

    // Identifier for thread
    pthread_t fervendo_agua;

    // Lança thread para ferver a agua
    pthread_create(&fervendo_agua, NULL, fervendo_agua_thread, (void*)agua);
    
    // Enquantoa agua ferve, cria e corta os legumes
    legumes_t* legumes = create_legumes();
    cortar_legumes(legumes);

    // Pega a agua que estava fervendo
    pthread_join(fervendo_agua, NULL);

    // Pega boca de fogão para fazer o caldo
    sem_wait(&s_bocas);

    // Prepara o caldo
    caldo_t* caldo = preparar_caldo(agua);

    // Cozinhar os legumes no caldo
    cozinhar_legumes(legumes, caldo);
    
    // Libera a boca do fogão
    sem_post(&s_bocas);

    // Emprata sopa
    empratar_sopa(legumes, caldo, prato);

    // Espera  espaço no balcão
    sem_wait(&s_balcao_cozinheiros);

    // Mexendo no balcão
    pthread_mutex_lock(&mutex_entrega_cozinheiro);
    pratos_prontos[index_cozinheiro] = prato;
    index_cozinheiro = (index_cozinheiro + 1) % balcao;
    pthread_mutex_unlock(&mutex_entrega_cozinheiro);

    // Notifica pedido
    notificar_prato_no_balcao(prato);

    // Diz para o garçons q tem prato no balcão
    sem_post(&s_balcao_garcons);
}

void prepara_spaghetti(pedido_t pedido) {
    pthread_t esquentando_molho, dourando_bacon, fervendo_agua;

    prato_t* prato = create_prato(pedido);

    // Cria o molho
    molho_t* molho = create_molho();

    // Bota o molho para esquentar
    pthread_create(&esquentando_molho, NULL, esquentando_molho_thread, (void*)molho);
    
    // Cria agua
    agua_t* agua = create_agua();

    // Bota agua pra ferve
    pthread_create(&fervendo_agua, NULL, fervendo_agua_thread, (void*)agua);
    
    // Cria bacon
    bacon_t* bacon = create_bacon();

    // Bota bacon para dourar
    pthread_create(&dourando_bacon, NULL, dourando_bacon_thread, (void*)bacon);

    pthread_join(esquentando_molho, NULL);
    pthread_join(fervendo_agua, NULL);
    pthread_join(dourando_bacon, NULL);

    spaghetti_t* spaghetti = create_spaghetti();
    
    // Pega boca do fogão para cozinhar o spaghetti
    sem_wait(&s_bocas);

    cozinhar_spaghetti(spaghetti, agua);

    // Joga a água fora
    destroy_agua(agua);

    // Libera boca do fogão
    sem_post(&s_bocas);

    empratar_spaghetti(spaghetti, molho, bacon, prato);

    // Espera espaço no balcão
    sem_wait(&s_balcao_cozinheiros);
    
    // Mexendo no balcão
    pthread_mutex_lock(&mutex_entrega_cozinheiro);
    pratos_prontos[index_cozinheiro] = prato;
    index_cozinheiro = (index_cozinheiro + 1) % balcao;
    pthread_mutex_unlock(&mutex_entrega_cozinheiro);

    notificar_prato_no_balcao(prato);

    // Avisa garcons
    sem_post(&s_balcao_garcons);
}

void* cozinheiro_thread(void* arg) {
    pedido_t* pedido = ((pedido_t*) arg);
    switch(pedido->prato) {
		case PEDIDO_SPAGHETTI:
            prepara_spaghetti(*pedido);
			break;	

		case PEDIDO_SOPA: 
			prepara_sopa(*pedido);
			break;

		case PEDIDO_CARNE:
			prepara_carne(*pedido);
			break;

        case PEDIDO__SIZE:
		case PEDIDO_NULL:
            // Have a break, have a Kit Kat
			break;
	}
    // Desalocando pedido
    free(pedido);
    // Post para ter cozinheiro livre
    sem_post(&s_cozinheiros);
    
    pthread_exit(0);
}

void* garcon_thread(void* arg) {
    // Trabalho escravo
    while (1) {
        // Alocar o espaço para colocar o prato, para ser deletado
        // Já que nosso balcão em forma array será desalocado no destroy cozinha
        prato_t* prato;// = (prato_t*) malloc(sizeof(prato_t));

        // Espera ter algo pra pegar
        sem_wait(&s_balcao_garcons);
        pthread_mutex_lock(&mutex_coleta_garcon);
        // Checa indice a coletar
        prato = pratos_prontos[index_garcon];
        index_garcon = (index_garcon+1) % balcao;
        // Demite o garçon se for pedido nulo
        if(prato->pedido.prato == PEDIDO_NULL) {
            // Jogando fora o prato nulo da demissão
            // Pq se cada garçon não se livrar do próprio
            // prato, a thread principal não vai fazer
            free(prato);

            // Destrancando o balcão
            pthread_mutex_unlock(&mutex_coleta_garcon);

            // Avisando os cozinheiros q liberou um prato
            sem_post(&s_balcao_cozinheiros);
            pthread_exit(0);
        }

        pthread_mutex_unlock(&mutex_coleta_garcon);

        // Abre espaço para cozinheiros
        sem_post(&s_balcao_cozinheiros);
        entregar_pedido(prato);
    }
}
