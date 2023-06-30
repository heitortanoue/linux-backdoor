# Módulo de Kernel para Leitura de Teclado e Envio via Socket

Este é um módulo de kernel para Linux que permite a leitura de eventos do teclado e envia esses eventos para outra máquina por meio de um socket de rede.

## Pré-requisitos

- Sistema operacional Linux.
- Ambiente de desenvolvimento de kernel configurado.

## Instalação

1. Faça o download do código-fonte do módulo para a sua máquina.
2. Abra um terminal e navegue até o diretório onde o código-fonte do módulo está localizado.
3. Compile o módulo executando o comando `make`.
4. Carregue o módulo no kernel usando o comando `make install`.
5. Verifique os logs do kernel para garantir que o módulo tenha sido carregado corretamente.

## Uso

Após a instalação e carregamento do módulo, ele começará a capturar os eventos do teclado. Os eventos serão enviados para outra máquina por meio de um socket de rede. Para utilizar os dados enviados, é necessário implementar um servidor socket na máquina de destino.

## Configuração do Servidor Socket

1. Na máquina de destino, implemente um servidor socket para receber os eventos do teclado.
2. Configure o servidor para aguardar conexões de rede na porta especificada no módulo de kernel.
3. Receba os dados enviados pelo módulo e processe-os de acordo com suas necessidades.

## Desinstalação

1. Para remover o módulo do kernel, execute o comando `make uninstall`.
2. Verifique os logs do kernel para garantir que o módulo tenha sido removido corretamente.

## Notas

- Este módulo é fornecido apenas para fins educacionais e de demonstração. O uso indevido de recursos de captura de teclado é estritamente proibido e pode violar a privacidade das pessoas.
- Certifique-se de seguir todas as leis e regulamentações aplicáveis ao utilizar esse módulo.
- Os autores não se responsabiliza por qualquer uso indevido ou ilegal deste módulo.

## Autores

- Beatriz Cardoso de Oliveira
- Beatriz Aimee Teixeira Furtado Braga
- Heitor de Mello Tanoue
