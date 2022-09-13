/**
 * @file marketplace.cpp
 * @author Isaac Franco (isaacfranco@imd.ufrn.br)
 * @version 0.1
 * @date 2022-01-27
 * 
 * @brief Marketplace em C++
 * 
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "utils.h"

using namespace std;

class Usuario {
    public:
    int id; // número incremental
    string email;
    string nome;
    string senha_hash; // Senha em hash
};

class Produto {
    public:
    int id; // número incremental
    string nome;
    float preco;
    int quantidade;
};

class Loja {
    public:
    int id; // número incremental
    string nome;
    Usuario proprietario;
    vector<Produto> produtos;
};

class Venda {
    public:
    int id; // número incremental
    int comprador_id; // Id do Usuário que fez a compra
    int loja_id; // Id da Loja que o produto foi comprado
    int produto_id; // Id do produto comprado
    int quantidade; // Quantos produtos foram comprados
    float preco_unitario; // Qual era o preço do produto no momento da venda
};


class Marketplace {
    private:
    map<string, Usuario> usuarios; // Chave: email, Valor: Usuario
    map<int, Loja> lojas; // Chave: id da loja, Valor: Loja
    
    map<string, int> acessos_liberados; // Chave: token_de_acesso, Valor: id_do_usuario
    
    vector<Venda> vendas;
    int ultimo_produto_id = 0;
    int ultima_venda_id = 0;
    // TODO Separa a implementação em .h e .cpp do Marketplace
    public:
        Marketplace() {

        }

        int token_verify(string token_de_acesso){
            for (auto &i : acessos_liberados){
                if(i.first == token_de_acesso){
                    return i.second;
                }
            }
            return 0;
        }

        Usuario usuario_por_id(int id){
            for (auto it : usuarios){
                if(it.second.id == id){
                    return it.second;
                }
            }
            return Usuario();
        }

        /**
         * Cadastra um usuário no marketplace, retornando true ou false se o cadastro foi realizado com sucesso.
         * O e-mail deve ser único
         * @param nome Nome do usuário
         * @param email Email do usuário
         * @param senha Senha do usuário. Deve ser armazenada em forma criptografada.
         * @return True se o cadastro foi realizado com sucesso, false caso contrário.
         */
        bool me_cadastrar(string nome, string email, string senha) {
            // TODO(opcional) Implementar
            // Buscando usuário com e-mail no cadastro
            auto it = usuarios.find(email);
            // Se não existir, cria um novo usuário
            if (it == usuarios.end()) {
                Usuario novo_usuario;
                novo_usuario.id = usuarios.size() + 1; //podemos fazer assim pois não existe remoção
                novo_usuario.email = email;
                novo_usuario.nome = nome;
                novo_usuario.senha_hash = geraHash(senha);
                usuarios.insert(make_pair(email, novo_usuario));
                return true;
            }
            return false;
        }

        /**
         * Tenta logar o usuário com esse e-mail / senha.
         * Caso bem sucessido o login, deve gerar aleatoriamente um token de acesso
         * e o par <token, usuario_id> deve ser armazenado em "acessos_liberados".
         * @param email Email do usuário
         * @param senha Senha do usuário.
         * @return  token de acesso caso o login seja bem sucedido. Caso contrário, retornar "invalid"
         */
        string login(string email, string senha) {
            // TODO(opcional) Implementar
            // Buscando usuário com e-mail no cadastro
            auto it = usuarios.find(email);
            // Se não existir, retorna "invalid"
            if (it == usuarios.end()) {
                return "invalid";
            }
            // Se existir, verifica se a senha está correta
            string senha_hash = geraHash(senha);
            if (it->second.senha_hash == senha_hash) {
                // Se estiver correta, gera um token de acesso
                string token_de_acesso = genRandomString(20);
                // Armazena o token de acesso e o id do usuário
                acessos_liberados.insert(make_pair(token_de_acesso, it->second.id));
                return token_de_acesso;
            }
            return "invalid";
        }

        

        /**
         * Cria uma loja no marketplace com o nome especificado para o usuário que tem
         * um acesso com esse token.
         * @param token Token de acesso
         * @param nome Nome da loja
         * @return O id da loja, ou -1 caso o token não exista em acessos_liberados ou
         * uma loja com esse nome já exista no marketplace
         */
        int criar_loja(string token, string nome) {
            // TODO Implementar
            int id_usuario = token_verify(token);
            if (id_usuario > 0){
                Loja nova_loja;
                nova_loja.proprietario = usuario_por_id(id_usuario);
                nova_loja.nome = nome;
                nova_loja.id = lojas.size() +1; //podemos fazer assim pois não existe remoção, apenas deslocamento
                lojas.insert(make_pair(nova_loja.id, nova_loja));
                cout << "Cadastrando..  " << nova_loja.nome << " | de id: " << nova_loja.id << endl;
                return nova_loja.id;
            }else{
                return -1;
            }
        }

        /**
         * Adicionando produtos em uma loja(pelo id) de um usuário(pelo token).
         * Não é permitido adicionar um produto em um loja caso seu proprietário não seja o usuário do token passado
         * A quantidade de um produto inserido é 0 (zero)
         * 
         * @return Um id do produto adicionado para ser usado em outras operações
         */
        int adicionar_produto(string token, int loja_id, string nome, float preco) {
            // TODO Implementar
            if(token_verify(token)){
                for (auto &i : lojas){
                    if(i.first == loja_id){
                        Produto novo_produto;
                        novo_produto.id = ultimo_produto_id++; //podemos fazer assim pois não existe remoção
                        novo_produto.nome = nome;
                        novo_produto.preco = preco;
                        novo_produto.quantidade = 0;
                        i.second.produtos.push_back(novo_produto);
                        cout << "Produto inserido com sucesso. (" << nome << ")" << endl;
                        return novo_produto.id;
                    }
                }
            }else{
                return -1;
            }
        }

        /////////////////nome.find(nome_parcial) != string::npos
        /**
         * Adiciona uma quantidade em um produto em uma loja(pelo id) de um usuário(pelo token).
         * 
         * @param token Token de acesso
         * @param loja_id Id da loja
         * @param produto_id Id do produto
         * @param quantidade Quantidade a ser adicionada
         * @return retornar novo estoque
         */
        int adicionar_estoque(string token, int loja_id, int produto_id, int quantidade) {
            // TODO Implementar
            if(token_verify(token)){
                for (auto &i : lojas){ //map<int, Loja> lojas | id, Loja
                    if(i.first == loja_id){ //id == loja_id
                        vector<Produto>::iterator it;
                        for(auto it = i.second.produtos.begin(); it != i.second.produtos.end(); it++){
                            if(it->id == produto_id){
                                //cout << it->quantidade << endl;
                                it->quantidade += quantidade;
                                return it->quantidade;
                            }
                        }  
                    }
                }
            
            }
            
            return -1;
        }


        

        /**
         * Muda um produto da loja com o id loja_origem_id para loja_destino_id
         * Garantir que:
         *  - loja_origem_id e loja_destino_id são do usuário
         *  - O produto está originalmente na loja_origem
         *  - loja_origem_id != loja_destino_id
         * 
         * @param token Token de acesso
         * @param loja_origem_id Id da loja de origem
         * @param loja_destino_id Id da loja de destino
         * @param produto_id Id do produto
         * @return True se a operação foi bem sucedida, false caso contrário
         */
        bool transferir_produto(string token, int loja_origem_id, int loja_destino_id, int produto_id) {
            // TODO Implementar
            if(token_verify(token)){
                if(loja_origem_id != loja_destino_id){
                    for (auto &i : lojas){
                        if(i.second.id == loja_origem_id){
                            for(auto &f : i.second.produtos){
                                if(f.id == produto_id){
                                    f.quantidade--;

                                }   
                            }
                        }
                        else if(i.second.id == loja_destino_id){
                            for(auto &f : i.second.produtos){
                                if(f.id == produto_id){
                                    f.quantidade++;

                                }   
                            }
                        }
                    }
                }
            }    
            return false;
        }

        /**
         * Lista de produtos do marketplace que tem a string nome_parcial no nome
         * 
         * @param nome_parcial String que deve aparecer no nome do produto
         * @return Lista de produtos que tem a string nome_parcial no nome
         */
        vector<Produto> buscar_produtos(string nome_parcial) {
            vector<Produto> encontrados;
            for (auto &i : lojas){
                for(auto &f : i.second.produtos){
                    if (f.nome.find(nome_parcial) != string::npos){
                        encontrados.push_back(f);
                    }
                }
            }
            return encontrados;
        }

        /**
         * Lista de produtos de uma loja específica do marketplace que tem a string nome_parcial no nome
         * 
         * @param nome_parcial String que deve aparecer no nome do produto
         * @param loja_id Id da loja
         * @return Lista de produtos que tem a string nome_parcial no nome e que pertencem a loja especificada
         */
        vector<Produto> buscar_produtos(string nome_parcial, int loja_id) {
            vector<Produto> encontrados;
            for (auto &i : lojas){
                if(i.first == loja_id){
                    for(auto &f : i.second.produtos){
                        if (f.nome.find(nome_parcial) != string::npos){
                            encontrados.push_back(f);
                        }
                    }
                }
            }
            return encontrados;
        }

        /**
         * Lista de lojas do marketplace que tem a string nome_parcial no nome
         * 
         * @param nome_parcial String que deve aparecer no nome da loja
         * @return Lista de lojas que tem a string nome_parcial no nome
         */
        vector<Loja> buscar_lojas(string nome_parcial) {
            vector<Loja> encontradas;
            for (auto &i : lojas){
                if (i.second.nome.find(nome_parcial) != string::npos){
                    encontradas.push_back(i.second);
                }
            }
            return encontradas;
        }

        /**
         * Lista de lojas do marketplace
         * 
         * @return Lista de lojas do marketplace
         */
        vector<Loja> listar_lojas() {
            vector<Loja> encontradas;

            for (auto &&i : lojas){
                encontradas.push_back(i.second);
            }

            return encontradas;
        }

        /**
         * Cria uma nova Venda para o usuário com acesso com esse token,
         * para o produto especificado, para a loja desse produto e com a quantidade especificada.
         * 
         * @param token Token de acesso
         * @param produto_id Id do produto
         * @param quantidade Quantidade a ser vendida
         * @return Id da venda criada ou -1 caso não seja possível criar a venda
         */
        int comprar_produto(string token, int produto_id, int quantidade) {
            
            int id_usuario = token_verify(token);
            if(id_usuario > 0){
                Venda venda;
                venda.id = ultima_venda_id++;
                venda.comprador_id = id_usuario;
                venda.quantidade = quantidade;
                for (auto i : lojas){
                    for(auto it = i.second.produtos.begin(); it != i.second.produtos.end(); it++){
                        if(it->id == produto_id){
                            venda.loja_id = i.first;
                            venda.preco_unitario = it->preco;
                        }
                    }
                }
            return venda.id;
            }
        }


        // Métodos de debug (adicionar a vontade)
        void show_usuarios() {
            for (auto it = usuarios.begin(); it != usuarios.end(); it++) {
                cout << it->first << " >>> " << it->second.senha_hash << endl;
            } cout << endl;
        }
        void show_tokens() {
            for (auto it = acessos_liberados.begin(); it != acessos_liberados.end(); it++) {
                cout << it->first << " >>> " << it->second << endl;
            } cout << endl;
        }

        void show_all(){
            cout << "lojas" << endl;
            for(auto i : lojas){
                cout << i.second.nome << endl;
                cout << i.second.produtos.size() << endl;
            }
        }

};


int main() {
    Marketplace marketplace;

    bool cadastro1_ok, cadastro2_ok;
    cout << endl << "=~= TESTE - CADASTRO =~=~=~=~=~=~=~=~=~==~=~=~=~=~=~=~=~=~=" << endl << endl;
    cadastro1_ok = marketplace.me_cadastrar("João", "joao@gmail.com", "123456");
    testa(cadastro1_ok, "Cadastro de usuário 1");
    cadastro2_ok = marketplace.me_cadastrar("Maria", "maria@gmail.com", "654321");
    testa(cadastro2_ok, "Cadastro de usuário 2");

    cout << "=~= TESTE - USUARIOS =~=~=~=~=~=~=~=~=~==~=~=~=~=~=~=~=~=~=" << endl << endl;
    marketplace.show_usuarios();

    if ( !cadastro1_ok || !cadastro2_ok) {
        cout << "Cadastro de João ou Maria nao realizado" << endl;
        return -1;
    }

    cout << endl << "=~= TESTE - TOKENS =~=~=~=~=~=~=~=~=~==~=~=~=~=~=~=~=~=~=" << endl << endl;
    //string invalid_token = marketplace.login("inexistente@hotmail.com", "senha qualquer");
    //testa(invalid_token == "invalid", "login de usuario inexistente");
    
    string joao_token = marketplace.login("joao@gmail.com", "123456");
    testa(joao_token != "invalid", "login de usuario valido");
    cout << "Token de acesso recebido para João: " << joao_token << endl;

    string maria_token = marketplace.login("maria@gmail.com", "654321");
    testa(maria_token != "invalid", "login de usuario valido");
    cout << "Token de acesso recebido para Maria: " << maria_token << endl;

    cout << endl << "=~= TESTE - MARKETPLACE =~=~=~=~=~=~=~=~=~==~=~=~=~=~=~=~=~=~=" << endl << endl;
    cout << "Login do usuário..." << endl;
    if (joao_token != "invalid") {
        cout << "Usuário logado com sucesso" << endl;

        cout<< endl  << "=~= Teste de cadastro de loja pelo Usuário logado =~=~=~=~=~=~=" << endl << endl;
        // João cria duas lojas com o seu token de acesso
        int bodega_do_joao_id = marketplace.criar_loja(joao_token, "Bodega do João");
        testa(bodega_do_joao_id != -1, "Cadastro de loja");
        int acougue_do_joao_id = marketplace.criar_loja(joao_token, "Açougue do João");
        testa(acougue_do_joao_id != -1, "Cadastro de loja");
        int bodega_da_maria_id = marketplace.criar_loja(maria_token, "Bodega da Maria");
        testa(bodega_da_maria_id != -1, "Cadastro de loja");
        
        cout<< endl  << "=~=Teste de adição de produto à loja pelo Usuário logado =~=~=~=~=~=~=" << endl << endl;
        // João adiciona produtos na loja e altera o seu estoque
        int leite_id = marketplace.adicionar_produto(joao_token, bodega_do_joao_id, "Leite em pó", 8.40);
        testa(leite_id != -1, "Cadastro de produto");
        marketplace.adicionar_estoque(joao_token, bodega_do_joao_id, leite_id, 10);
        
        cout<< endl  << "=~= Teste de incremento de estoque pelo Usuário logado =~=~=~=~=~=~=" << endl;
        int novo_estoque_leite = marketplace.adicionar_estoque(joao_token, bodega_do_joao_id, leite_id, 5);
        testa(novo_estoque_leite == 15, "Adicionando estoque");
        
        int arroz_id = marketplace.adicionar_produto(joao_token, bodega_do_joao_id, "Arroz", 3.50);
        marketplace.adicionar_estoque(joao_token, bodega_do_joao_id, arroz_id, 40);
        
    
        int coca_id = marketplace.adicionar_produto(joao_token, bodega_do_joao_id, "Coca cola 250ml", 2.40);
        marketplace.adicionar_estoque(joao_token, bodega_do_joao_id, coca_id, 15);
        
        // João adicionou um produto na loja que não deveria... Aff João...
        int picanha_id = marketplace.adicionar_produto(joao_token, bodega_do_joao_id, "Picanha Maturada", 58.40);
        marketplace.adicionar_estoque(joao_token, bodega_do_joao_id, picanha_id, 5);
        
        int pic_suina_id = marketplace.adicionar_produto(joao_token, acougue_do_joao_id, "Picanha Suína", 78.40);
        marketplace.adicionar_estoque(joao_token, acougue_do_joao_id, pic_suina_id, 8);
        
        cout<< endl  << "=~= Teste de transferência de produto =~=~=~=~=~=~=" << endl;

        // Transferindo um produto de uma loja para outrao (do mesmo usuário)
        marketplace.transferir_produto(joao_token, bodega_do_joao_id, acougue_do_joao_id, pic_suina_id);

        cout << endl << "~~~~~ Não consegui executar bem! ~~~~~" << endl;
        cout<< endl  << "=~= Teste de venda de produto =~=~=~=~=~=~=" << endl;

        marketplace.comprar_produto(joao_token, picanha_id, 1);
        marketplace.comprar_produto(joao_token, coca_id, 1);
        marketplace.comprar_produto(joao_token, arroz_id, 1);
        int compra = marketplace.comprar_produto(joao_token, leite_id, 1);
        testa(compra != -1, "Comprando produto");
        
        cout<< endl  << "=~= Teste de outro login e exibição dos tokens =~=~=~=~=~=~=" << endl << endl;
        // Logar como Maria
        if (maria_token != "invalid"){ //string maria_token = marketplace.login("maria@gmail.com", "654321");
        cout << "Usuário logado com sucesso" << endl; //testa(maria_token != "invalid", "login de usuario valido");
            cout << endl;
            marketplace.comprar_produto(maria_token, pic_suina_id, 2);

            int oleo_id = marketplace.adicionar_produto(maria_token, bodega_da_maria_id, "Oleo", 9.69);
            marketplace.adicionar_estoque(maria_token, bodega_da_maria_id, oleo_id, 5);
            cout << endl;

            // Maria buscando picanha:
            vector<Produto> picanhas = marketplace.buscar_produtos("Picanha");
            cout << "Quantidade de picanhas encontradas: " << picanhas.size() << endl;
            // verificando se achou picanhas e comprando a primeira
            if (picanhas.size() > 0) {
                marketplace.comprar_produto(maria_token, picanhas[0].id, 1);
            }

            // Maria buscando uma loja qualquer com o nome Bodega
            vector<Loja> bodegas = marketplace.buscar_lojas("Bodega");
            // vendo se existe alguma bodega, se existir, comprando o primeiro produto com o nome Coca
            cout << "Quantidade de bodegas encontradas: " << bodegas.size() << endl;
            if (bodegas.size() > 0) {
                vector<Produto> produtos = marketplace.buscar_produtos("Coca", bodegas[0].id);
                cout << "Quantidade de Cocas encontradas na bodega: " << produtos.size() << endl;
                if (produtos.size() > 0) {
                    marketplace.comprar_produto(maria_token, produtos[0].id, 1);
                }
            }
        }
        cout << endl << "Tokens e seus id's de usuário: " << endl;
        marketplace.show_tokens(); // opcional. debug
        
        cout << "=~= Teste de exibição de lojas cadastradas =~=~=~=~=~=~=" << endl << endl;
        // mostrando todas as lojas do marketplace
        vector<Loja> lojas = marketplace.listar_lojas();
        for (Loja loja : lojas) {
            cout << loja.nome << ":" << endl;
            for(Produto produto : loja.produtos){
                cout << "- " << produto.nome << "; \t" << endl;
            }
            cout << endl;
        }
        
    } else {
        cout << "Usuário não pode se logar" << endl;

        
    }
}

