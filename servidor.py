# servidor.py
from concurrent import futures
import grpc
import hashlib
import time

# Importa as classes geradas
import urls_pb2
import urls_pb2_grpc

class EncurtadorURLServicer(urls_pb2_grpc.EncurtadorURLServicer):
    """
    Implementa a lógica do serviço de encurtamento de URL.
    """
    def __init__(self):
        # Dicionário em memória para armazenar as URLs. 
        self.url_map = {}

    def EncurtarURL(self, request, context):
        """
        Recebe uma URL longa, gera um código curto e armazena a associação. [cite: 42]
        """
        url_longa = request.url_longa # [cite: 43]
        
        # Gera um código curto usando hash da URL + tempo atual. [cite: 44, 69]
        hash_object = hashlib.sha1(f"{url_longa}{time.time()}".encode())
        codigo_curto = hash_object.hexdigest()[:7]

        # Armazena a associação no dicionário. [cite: 45]
        self.url_map[codigo_curto] = url_longa
        print(f"URL encurtada: {url_longa} -> {codigo_curto}")
        
        # Retorna a URL curta completa. [cite: 47]
        # A porta 50051 é usada como exemplo. [cite: 48]
        url_curta_completa = f"http://localhost:50051/{codigo_curto}"
        return urls_pb2.RespostaEncurtar(url_curta=url_curta_completa)

    def ObterURLLonga(self, request, context):
        """
        Recebe um código curto e retorna a URL longa original. [cite: 49]
        """
        codigo_curto = request.codigo_curto # [cite: 50]
        
        # Busca o código no dicionário. [cite: 51]
        url_longa = self.url_map.get(codigo_curto, "") # Retorna string vazia se não encontrar. [cite: 52]
        
        if url_longa:
            print(f"URL longa encontrada para o código {codigo_curto}")
        else:
            print(f"Nenhuma URL encontrada para o código {codigo_curto}")
            
        return urls_pb2.RespostaObter(url_longa=url_longa)

def serve():
    """
    Inicia o servidor gRPC e o faz escutar na porta 50051. [cite: 53]
    """
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    urls_pb2_grpc.add_EncurtadorURLServicer_to_server(EncurtadorURLServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print("Servidor gRPC rodando na porta 50051...")
    server.wait_for_termination()

if __name__ == '__main__':
    serve()