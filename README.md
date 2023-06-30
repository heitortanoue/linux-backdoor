# Módulo de Kernel para Leitura de Teclado e Snapshot da memória de vídeo com Envio via Socket

Este é um módulo de kernel para Linux que permite a leitura de eventos do teclado, bem como a tela e envia esses eventos para outra máquina por meio de um socket de rede.

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
- Os autores não se responsabilizam por qualquer uso indevido ou ilegal deste módulo.

## Autores

- Beatriz Cardoso de Oliveira - 12566400.
- Beatriz Aimee Teixeira Furtado Braga - 12547934.
- Heitor de Mello Tanoue - 12547260.

## Versões Anteriores (Deprecated)

Este repositório também inclui uma pasta chamada "deprecated" que contém versões anteriores do código que não funcionam corretamente. Nessa tentativa anterior, a leitura do snapshot da memória de vídeo é realizada usando o framebuffer `fb0`, e o servidor está incorporado no kernel. No entanto, essas versões anteriores apresentaram problemas e foram descartadas.

## Modificações no Código Final

O código final deste módulo inclui as seguintes modificações:

- Adição de um keylogger: o keylogger armazena as informações das teclas em um buffer e as envia quando o usuário pressiona a tecla "Enter".
- Envio de um print da tela a cada 10 segundos: a leitura da tela é realizada por meio de uma chamada de comando no terminal que tira o print e, portanto, é executada no userspace.
- O servidor desta versão também está no userspace.

## Código de Recepção

Além disso, no repositório, você encontrará um arquivo chamado "receiver.c" que é responsável pelo recebimento dos eventos enviados e deve ser compilado usando a flag "-lX11". Este arquivo contém a lógica para receber os dados do módulo e processá-los de acordo com as necessidades do usuário.

https://github.com/totoi690/linux-backdoor/tree/master
