#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_C 256                //定义最大字符数
#define MAX_N 512                //定义最大Huffman节点个数
#define N 50

/*Huffman Tree 结构*/
typedef struct
{   char ch;                        //字符
    int weight;                     //字符权重
    int lchild;                     //左子
    int rchild;                     //右子
    int parent;                     //双亲
}THNODE;
typedef THNODE HuffmanT[MAX_N];

/******Huffman 编码表结构*****/
typedef struct
{   char ch;                         //存储字符
    char bits[MAX_C + 1];         //字符编码位串
}CodeNode;

typedef CodeNode HuffmanCode[MAX_C];
HuffmanCode H;

/*********全局变量**********/
int n;                  //指示待编译文件的字长
char filename[20];

/*初始化Huffman树*/
void InitHT(HuffmanT T)
{   int i;
    for (i = 0; i < MAX_N; i++)
    {   T[i].ch = '\0';
        T[i].weight = 0;
        T[i].lchild = -1;
        T[i].rchild = -1;
        T[i].parent = -1;
    }
}
/*找到权重最小的叶子*/
void SelectMin(HuffmanT T, int n, int &p1, int &p2)
{   int i;
    int j;
    for (i = 0; i < 2 * n -1; i++)
    {
        if (T[i].parent == -1 && T[i].weight > 0)
        {
            p1 = i;
            break;
        }
    }
    for (j = i + 1; j < 2 * n - 1; j++)
    {
        if (T[j].parent == -1 && T[j].weight > 0)
        {
            p2 = j;
            break;
        }
    }
    for (i = 0; i < 2 * n - 1; i++)
    {
        if ((T[p1].weight > T[i].weight) && (T[i].parent == -1) && (p2 != i) && (T[i].weight > 0))
            p1 = i;
    }
    for (j = 0; j < 2 * n - 1; j++)
    {
        if ((T[p2].weight > T[j].weight) && (T[j].parent == -1) && (p1 != j) && (T[j].weight > 0))
            p2 = j;
    }
}
/*******加载文件*******/
void LoadHuffmanFile(HuffmanT T)
{   unsigned int i;
    int j = 0;
    char c;
    int a[MAX_C];
    FILE *fp;
    printf("Input file name: ");
    scanf("%s", filename);
    if ((fp = fopen(filename, "rb")) == NULL)
    {   printf("Can't open %s\n", filename);
        exit( 0 );
    }
    for (i = 0; i < MAX_C; i++)
        a[i] = 0;
    fseek(fp, 0, SEEK_SET);
    while ( 1 )//( !feof(fp) )
    {
        fread(&c, sizeof(unsigned char), 1, fp);
        if (feof(fp))    break;
        a[(unsigned int)c]++;
    }
    fclose(fp);
    /*统计输入文件的字符及其权重并存放到树T[]*/
    for (i = 0; i < MAX_C; i++)
    {
        if (a[i] != 0)
        {
            T[j].ch = (unsigned char)i;
            T[j++].weight = (unsigned int)a[i];
        }
    }
    n = j;
}
/*******构造huffam树，T[2 * n - 1]为其根********/
void CreatHT(HuffmanT T)
{
    int i,p1,p2;
    LoadHuffmanFile(T);                     //加载被编码文件
    for (i = n; i < 2 * n - 1; i++)
    {   SelectMin(T, i - 1, p1, p2);
        T[p1].parent = T[p2].parent = i;
        T[i].lchild = p1;
        T[i].rchild = p2;
        T[i].weight = T[p1].weight + T[p2 ].weight;
    }
}
/*根据Huffman T 求Huffman编码表H*/
void CharSetHuffmanEncoding(HuffmanT T, HuffmanCode H)
{   int c;                             //指示T中孩子的位置
    int p;                             //指示T中双亲的位置
    int i;
    int start;                         //指示编码在cd中的位置
    char cd[N];                    //临时存放编码
    for (i = 0; i < n; i++)            //依次求叶子的编码
    {   H[i].ch = T[i].ch;             //读入叶子T[i]对应的字符
        if (i==0 || i==1) start=n;
        else start=n-i+1;
        cd[--start]='\0';                    //编码起始位置的初值
        c = i;                         //从叶子T[i]开始回溯
        while ((p = T[c].parent) >= 0) //直到回溯到T[c]是树根位置
        {   cd[--start] = (T[p].lchild == c) ? '0' : '1';
            c = p;}
        strcpy(H[i].bits, &cd[start]); //复制临时编码到编码表中
    }
}

/*对文件编码，将结果保存到codefile.txt中*/
void EncodingHuffmanT(HuffmanT T, HuffmanCode H)
{   char c;
    FILE *in,*fp;
    int j,l;
    char encodefile[20],temp[MAX_C];
    if ((in = fopen(filename, "rb")) == NULL)
    {   printf("Read %s fail!\n", encodefile);
        exit(1);}
    CharSetHuffmanEncoding(T, H);
    printf("Input encode file name: ");
    gets( encodefile );
    if ((fp = fopen(encodefile, "wb")) == NULL)
    {   printf("Write %s fail!\n", encodefile);
        exit(1);}
    fread(&c, sizeof(unsigned char), 1, in);
    fwrite(&c, sizeof(unsigned char), 1, fp);
    fseek(in, 0, SEEK_SET);
    fseek(fp, 0, SEEK_SET);
    while ( 1 )//( !feof( in ))
    {   fread(&c, sizeof(unsigned char), 1, in);
        if (feof(in))    break;
        for (j = 0; j < n; j++)
        {if (c == H[j].ch)
            {   l = 0;
                while (H[j].bits[l] != '\0')
                {   temp[l] = H[j].bits[l];
                    l++;}
                int m = 0;
                while ( l-- )
                {fwrite(&temp[m++], sizeof(unsigned char), 1, fp);}
            }
        }
    }
    fclose(fp);
    printf("Encoding file has saved into %s!\n", encodefile);
}
/*根据Huffman编码、译码*/
void DecodingHuffmanT(HuffmanT T, HuffmanCode H)
{   int i;           //指示Huffman tree叶子个数
    FILE *fp,*fp1;
    char ch,ch1[20],ch2[20];
    printf("Input encode file name: ");
    scanf("%s", ch1);
    printf("Input decode file name: ");
    scanf("%s", ch2);
    fp = fopen(ch1, "rb");
    fp1 = fopen(ch2, "wb");
    //根据Huffman树对Huffman编码 译码
    i = 2 * n - 2;
    fseek(fp, 0L, SEEK_SET);
    fseek(fp1, 0L, SEEK_SET);
    while (!feof(fp))
    {   fread(&ch, sizeof(unsigned char), 1, fp);
        if (ch == '0')                  //若编码为o，则找此结点的左子树；
            i = T[i].lchild;
        if (ch == '1')                  //若编码为１，则找此结点的右子树；
            i = T[i].rchild;
        if (i < n)
        {   fwrite(&T[i].ch, sizeof(unsigned char), 1, fp1);
            i = 2 * n - 2;
        }
    }
    fclose(fp);
    fclose(fp1);
    printf("Decoding accomplished!\nThe result has save input %s.\n",ch2);
    getchar();
}
/*打印Huffman权重表*/
void PrintHuffmanT(HuffmanT T)
{   int i;
    FILE *fp;
    if ((fp = fopen("treeprint.txt", "wb")) == NULL)
    {   printf("Open treeprint.txt fail!\n");
        exit(1);
    }
    printf("\nLeaf&weight of the Huffman tree is below:\n");
    for (i = 0; i < n ; i++)
    {       if (i % 5 == 0 && i > 0)
            printf("\n");
        if (T[i].weight > 0)
        {   fprintf(fp, "%c:%d  ", T[i].ch, T[i].weight);
            printf("%c: %d  ", T[i].ch, T[i].weight);
        }
    }
    fclose(fp);
    printf("\nLeaf&weight of the Huffman tree saved in treeprint.txt\n\n");
}
/*打印Huffman编码表*/
void PrintHuffmanH(HuffmanT T, HuffmanCode H)
{   int i;
    FILE *fp;
    CharSetHuffmanEncoding(T, H);
    if ((fp = fopen("codeprint.txt", "wb")) == NULL)
    {   printf("Open codeprint.txt fail!\n");
        exit(1);
    }
    for (i = 0; i < n; i ++)
    {   if (i % 10 == 0 && i > 0) printf("\n");
        printf("%c: %s\n", T[i].ch, H[i].bits);
        fprintf(fp, "%c:%s  ", T[i].ch, H[i].bits);
    }
    fclose(fp);
    printf("\nHuffman tree code saved in codeprint.txt!\n\n");
}
/*主菜单*/
void MainMenue()
{   fflush( stdin );
    printf("\n********************** Main Menue ***********************\n");
    printf("**                                                     **\n");
    printf("**              1. Load to be dealt file.              **\n");
    printf("**              2. Show Huffman code list.             **\n");
    printf("**              3. Show Huffman weight list.           **\n");
    printf("**              4. Encoding Huffman file.              **\n");
    printf("**              5. Decoding Huffman file.              **\n");
    printf("**              6. Exit.                               **\n");
    printf("**                                                     **\n");
    printf("*********************************************************\n");
}
/*主函数开始*/
int main()
{
    int flag = 1;
    char ch[10];
    HuffmanT T;                 //定义Huffman树
    HuffmanCode H;              //定义Huffman编码表
    InitHT(T);                  //初始化Huffman树
    while ( flag )
    {   MainMenue();
        printf("Please input your choice(1~6): ");
        gets( ch );
        switch ( ch[0] )
        {
            case '1':   CreatHT(T);                 break;
            case '2':   PrintHuffmanH(T, H);        break;
            case '3':   PrintHuffmanT(T);           break;
            case '4':   EncodingHuffmanT(T, H);     break;
            case '5':   DecodingHuffmanT(T, H);     break;
            case '6':   exit(1);
            default:    printf("Input error!\n");   break;
        }
    }
    return 0;
}