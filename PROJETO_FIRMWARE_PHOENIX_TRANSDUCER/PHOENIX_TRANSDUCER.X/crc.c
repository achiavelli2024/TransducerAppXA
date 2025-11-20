//
// Título:  ITIB1 - CRC Handler
// Versão:  1.0.0
// Criação: 21/Março/2013
// Lançamento:
//
//                             DESCRIÇÃO:
// Este arquivo contém o código para manipulação do módulo CRC da placa ITIB-1.
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
//  | 1.0.0    | 21/03/2013 | Reginaldo do Prado   | Criação do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//

#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
//
//
void CrcInit(void)
{
    IEC4bits.CRCIE = 0;                     //desabilita Interrupção do CRC Generator
    CRCCON1bits.CRCEN = 1;                  //Módulo CRC habilitado
    CRCCON1bits.CSIDL = 0;                  //Desabilita módulo CRC em Idle mode
    CRCCON1bits.CRCISEL = 0;
    CRCCON1bits.LENDIAN = 0;
    CRCCON2bits.DWIDTH = 0b00111;           //Dados de 8 bits
    CRCCON2bits.PLEN = 0b00111;             //Polinômio de 8 bits
    CRCXORH = 0x0000;
    CRCXORL = 0b0000000100100101;           //x^8 + x^5 + x^2 + 0
}

void CrcStart(void)
{
    CRCWDATL = 0x0000;
    CRCWDATH = 0x0000;
    CRCCON1bits.CRCGO = 1;
    while(CRCCON1bits.CRCGO);
    Nop();

}

