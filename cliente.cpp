// cliente.cpp
#include <iostream>
#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>

// Inclui os cabeçalhos gerados pelo protoc
#include "urls.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using encurtador::EncurtadorURL;
using encurtador::RequisicaoEncurtar;
using encurtador::RespostaEncurtar;
using encurtador::RequisicaoObter;
using encurtador::RespostaObter;

class URLCliente {
public:
    // Construtor que cria o canal e o stub gRPC. [cite: 58]
    URLCliente(std::shared_ptr<Channel> channel)
        : stub_(EncurtadorURL::NewStub(channel)) {}

    // Função para chamar o RPC EncurtarURL.
    std::string Encurtar(const std::string& url_longa) {
        RequisicaoEncurtar request;
        request.set_url_longa(url_longa);

        RespostaEncurtar reply;
        ClientContext context;

        // A chamada RPC real.
        Status status = stub_->EncurtarURL(&context, request, &reply);

        if (status.ok()) {
            return reply.url_curta();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "Falha na chamada RPC";
        }
    }

    // Função para chamar o RPC ObterURLLonga.
    std::string Obter(const std::string& codigo_curto) {
        RequisicaoObter request;
        request.set_codigo_curto(codigo_curto);

        RespostaObter reply;
        ClientContext context;

        // A chamada RPC real.
        Status status = stub_->ObterURLLonga(&context, request, &reply);

        if (status.ok()) {
            return reply.url_longa();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "Falha na chamada RPC";
        }
    }

private:
    std::unique_ptr<EncurtadorURL::Stub> stub_;
};

int main(int argc, char** argv) {
    // Cria um canal para se conectar ao endereço do servidor. [cite: 57]
    std::string server_address("localhost:50051");
    URLCliente cliente(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    std::string url_original = "https://docs.github.com/pt/codespaces/developing-in-codespaces/developing-in-a-codespace";

    // 1. Chama EncurtarURL e imprime o resultado. [cite: 60]
    std::string url_curta_completa = cliente.Encurtar(url_original);
    std::cout << "URL Original: " << url_original << std::endl;
    std::cout << "URL Curta recebida: " << url_curta_completa << std::endl;

    // Extrai o código curto da URL completa
    std::string codigo_curto = url_curta_completa.substr(url_curta_completa.find_last_of("/") + 1);

    // 2. Usa o código curto para obter a URL longa original e verificar. [cite: 61, 62]
    std::string url_longa_recuperada = cliente.Obter(codigo_curto);
    std::cout << "URL Longa recuperada: " << url_longa_recuperada << std::endl;

    // Verifica se o sistema funciona
    if (url_original == url_longa_recuperada) {
        std::cout << "\nA URL recuperada é igual à original." << std::endl;
    } else {
        std::cout << "\nA URL recuperada é diferente da original." << std::endl;
    }

    return 0;
}