//
// Título:  ITIB1 - Oscillator Handler
// Versão:  1.0.0
// Criação: 12/Março/2013
// Lançamento:
//
//                             DESCRIÇÃO:
// Este arquivo contém o código para configuração do oscilador e PLL da placa ITIB-1.
// Escrito para o PIC24EP512GP806 e compilador XC16.
//
// Copyright (C) 2013 M.Shimizu Elétrica e Pneumática Ltda. Todos os direitos reservados.
//
//                            AVISO LEGAL:
//
// Este arquivo e todo seu conteúdo é propriedade da M.Shimizu Elétrica e Pneumática Ltda.
// A publicação, distribuição ou modificações, totais or parciais, são expressamente proibidas
// sem autorição da M.Shimizu ou dos seus representantes legais.
//
// Para maiores informações viste www.mshimizu.com.br
//
//  +----------+------------+----------------------+-----------------------------+
//  | Versão   | Data       | Autor                | Comentário                  |
//  +----------+------------+----------------------+-----------------------------+
//  | 1.0.0    | 12/03/2013 | Reginaldo do Prado   | Criação do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"


void OscInit(void)
{
    //Cristal de 12MHz, HS Mode, Fin = 12MHz
    //N1 = 3, N2 = 2 e M = 70
    //Resulta:
    //
    //FREF = 4MHz
    //FVCO = 160MHz
    //Fosc = 140MHz
    //
    CLKDIV = 0x0001;            //N1 = 3 e N2 = 2
    PLLFBD = 0x0044;            //M = 70
    //
    //Neste momento o sistema ficará instável até a que a troca ocorra
    //Não continuar a execução do programa até a estabilização.
    //
    __builtin_write_OSCCONH(0x03);  //Nova origem do clock = HSPLL
    __builtin_write_OSCCONL(0x41);  //Inicia clock switching
    //
    //Neste ponto aguarda-se a troca da origem do clock do sistema.
    //Em caso de falha, o sistema ficará travado infinitamente até o próximo Reset.
    //Habilitar previamente o Watchdog Timer para que o sistema possa se recuperar.
    //
    // Aguarda a troca do clock
    while(OSCCONbits.COSC != 0b011);
    // Aguarda o PLL ser sincronizado
    while(OSCCONbits.LOCK != 1);
    //
    //Se chegou até aqui, o clock switching foi bem sucedido.
    //
}
