# Cliente-Servidor TCP/UDP
Este projeto implementa um modelo de comunicação cliente-servidor utilizando os protocolos TCP e UDP.

## Estrutura do Projeto
    Cliente-Servidor-TCP-UDP/
    │
    ├── tcp/                 # Implementação Cliente/Servidor TCP
    │   └── README.md        # Instruções de uso para TCP
    │
    └── udp/                 # Implementação Cliente/Servidor UDP
        └── README.md        # Instruções de uso para UDP

## Requisitos
- Sistema UNIX
- Compilador C (gcc)
- Pacote de desenvolvimento libc6-dev

## Funcionalidades
- **TCP**: Comunicação confiável entre cliente e servidor.
- **UDP**: Comunicação não confiável, mas rápida entre cliente e servidor.

## Notas Finais
Cada protocolo (TCP e UDP) possui características diferentes. O TCP garante a entrega ordenada dos pacotes, enquanto o UDP é mais rápido, mas não garante a entrega ou ordem dos pacotes. Escolha o protocolo que melhor atende às necessidades do seu projeto.
Se precisar de mais informações ou ajuda, não hesite em abrir uma issue ou pull request.

## Contribuição
1. Faça um fork do repositório.
2. Crie uma branch para sua feature (`git checkout -b feature/nova-feature`).
3. Commit suas mudanças (`git commit -am 'Adiciona nova feature'`).
4. Faça push para a branch (`git push origin feature/nova-feature`).
5. Abra um Pull Request.
