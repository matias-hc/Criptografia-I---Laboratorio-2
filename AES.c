#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define mssgLen 0x10

char defaultString[mssgLen*2+1] = "003C6E471F4E22740E081B3154590B1A";
unsigned char defaultMssg[mssgLen] = {0x00, 0x3C, 0x6E, 0x47, 0x1F, 0x4E, 0x22, 0x74, 0x0E, 0x08, 0x1B, 0x31, 0x54, 0x59, 0x0B, 0x1A};

unsigned char is_hex(char c);
unsigned char ch2uch(char c);
int usage(char* progName);
unsigned char *cli_opt(int argc, char *argv[]);

unsigned char *ByteSub(unsigned char *M);
unsigned char **ShiftRows(unsigned char *B);
unsigned char *MixCol(unsigned char *b);

unsigned char **S, **MC, **SR, St[0x100] =
{0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76
,0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0
,0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15
,0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75
,0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84
,0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf
,0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8
,0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2
,0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73
,0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb
,0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79
,0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08
,0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a
,0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e
,0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf
,0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16},
MCt[0x10] =
{0x02, 0x03, 0x01, 0x01,
0x01, 0x02, 0x03, 0x01,
0x01, 0x01, 0x02, 0x03,
0x03, 0x01, 0x01, 0x02},
SRt[0x10] =
{0x00,0x05,0x0a,0x0f,
0x04,0x09,0x0e,0x03,
0x08,0x0d,0x02,0x07,
0x0c,0x01,0x06,0x0b};

int main(int argc, char *argv[])
{
  unsigned char i, j, *A;
  S = (unsigned char **) malloc(sizeof(unsigned char *)*0x10);

  A = cli_opt(argc, argv);
  if (A == NULL)
    return 0;

  for (i = 0; i < 0x10; i++) {
    S[i] = (unsigned char *) malloc(sizeof(unsigned char)*0x10);
  }
  for (i = 0; i < 0x10; i++) {
    for (j = 0; j < 0x10; j++) {
      S[i][j] = St[i*(0x10)+j];
    }
  }  //S[][] queda como la caja S de rijndael (AES)
  MC = (unsigned char **) malloc(sizeof(unsigned char *)*0x4);
  for (i = 0; i < 0x4; i++) {
    MC[i] = (unsigned char *) malloc(sizeof(unsigned char)*0x4);
  }
  for (i = 0; i < 0x4; i++) {
    for (j = 0; j < 0x4; j++) {
      MC[i][j] = MCt[i*(0x4)+j];
    }
  }  //MC[][] queda como la matriz constante de MixColumn de rijndael (AES)
  SR = (unsigned char **) malloc(sizeof(unsigned char *)*0x4);
  for (i = 0; i < 0x4; i++) {
    SR[i] = (unsigned char *) malloc(sizeof(unsigned char)*0x4);
  }
  for (i = 0; i < 0x4; i++) {
    for (j = 0; j < 0x4; j++) {
      SR[i][j] = SRt[i*(0x4)+j];
    }
  }  //SR[][] son los cambios en ShiftRows

  // ronda del algoritmo
  unsigned char *B, **BSR, **C = (unsigned char **) malloc(sizeof(unsigned char *)*0x4);
  for (printf("\nA  =  "),i = 0; i < 0x10; i++) {
    printf("%02x  ",A[i]);
  }
  B = ByteSub(A);
  for (printf("\nB  =  "),i = 0; i < 0x10; i++) {
    printf("%02x  ",B[i]);
  }
  BSR = ShiftRows(B);
  for (printf("\nBSR=  "),i = 0; i < 0x4; i++) {
    printf("%02x  %02x  %02x  %02x  ",BSR[i][0],BSR[i][1],BSR[i][2],BSR[i][3]);
  }

  for (printf("\nC  =  "),i = 0; i < 0x4; i++) {
    C[i] = MixCol(BSR[i]);
    printf("%02x  %02x  %02x  %02x  ",C[i][0],C[i][1],C[i][2],C[i][3]);
  }
  printf("\n");
  return 0;
}

unsigned char *ByteSub(unsigned char *M)
{
  unsigned char a, b, c, i,
  *B = (unsigned char *) malloc(sizeof(unsigned char*)*0x10);
  for(i = 0; i < 0x10; i++)
  {
    a = M[i]/16; //0xA-
    b = M[i]%16; //0x-B
    c = S[a][b];
    B[i] = c;
  }
  return B;
}

unsigned char **ShiftRows(unsigned char *B)
{
  unsigned char i, j, **BSR = (unsigned char **) malloc(sizeof(unsigned char *)*0x4);
  for (i = 0; i < 0x4; i++) {
    BSR[i] = (unsigned char *) malloc(sizeof(unsigned char)*0x4);
  }
  for (i = 0; i < 0x4; i++) {
    for (j = 0; j < 0x4; j++) {
      BSR[i][j] = B[SR[i][j]];
    }
  }
  return BSR;
}

unsigned char *MixCol(unsigned char *b){
  unsigned char i, j, *C = (unsigned char *) malloc(sizeof(unsigned char)*0x4);
  for(i=0;i<4;i++){
    C[i]=MC[i][0]*b[0]+MC[i][1]*b[1]+MC[i][2]*b[2]+MC[i][3]*b[3];
  }
  return C;
}

unsigned char *cli_opt(int argc, char *argv[]){
  int option, is_str=1, i, j;
  char c;
  unsigned char *ans = defaultMssg;
  FILE *fp;
  while((option = getopt(argc, argv, ":hsf")) != -1){
    switch(option){
      case 'h':
        usage(argv[0]);
        return NULL;
      case 's':
        is_str = 1;
        break;
      case 'f':
        is_str = 0;
        break;
    }
  }
  if (is_str){
    if (optind < argc){
      ans = (unsigned char *) calloc(mssgLen, sizeof(unsigned char));
      for (i=0, j=0; i<mssgLen && j<mssgLen*2 && is_hex(argv[optind][j]); i+=1, j+=1){
        ans[i] = ch2uch(argv[optind][j])*0x10;
        j+=1;
        if (is_hex(argv[optind][j])){
          ans[i] += ch2uch(argv[optind][j]);
        }
        else
          break;
      }
      if (i < mssgLen)
        printf("error de entrada: %c (valor ascii: %d) no corresponde a un caracter hexadecimal. no se leyo el total de 128 bits (8 caracteres hex.)\n", argv[optind][j], argv[optind][j]);
    }
  }
  else{
    if (optind < argc){
      fp = fopen(argv[optind],"r");
      if(fp == NULL){
        perror("Error in opening file");
        return NULL;
      }
      c = fgetc(fp);
      for (i=0; i<mssgLen && is_hex(c); i+=1){
        ans[i] = ch2uch(c)*0x10;
        c = fgetc(fp);
        if (is_hex(c)){
          ans[i] += ch2uch(c);
          c = fgetc(fp);
        }
      }
      if (i < mssgLen)
        printf("error de entrada: %c (valor ascii: %d) no corresponde a un caracter hexadecimal. no se leyo el total de 128 bits (8 caracteres hex.)\n", c, c);
      fclose(fp);
    }
  }
  return ans;
}

int usage(char* progName){
  char *usage_mssg =
  /*AES.o*/ "[Opciones] [M]\n"
  "Opciones:\n"
  "-h                 Imprimir este mensaje de ayuda\n"
  "-s                 Input mensaje en el string M. Opcion default\n"
  "-f                 Input mensaje en el archivo M. opcion alterna a -s\n"
  "\n"
  "M := mensaje (en hexadecimal) o nombre de archivo, si no se incluye se utiliza\n"
  "el valor default:"
      ;
  printf("%s %s %s\n",progName, usage_mssg, defaultString);
  return 0;
}

unsigned char ch2uch(char c){
  if (c <= '9' && c >= '0')
    return c - '0';
  else{if (c <= 'f' && c >= 'a')
    return 0xa + c - 'a';
  else{if (c <= 'F' && c >= 'A')
    return 0xa + c - 'A';
  else{
    printf("error de entrada: %c (valor ascii: %d) no corresponde a un caracter hexadecimal.\n", c, c);
    return 0;
  }}}
}

unsigned char is_hex(char c){
  return ((c <= '9' && c >= '0') || (c <= 'f' && c >= 'a') || (c <= 'F' && c >= 'A'));
}
