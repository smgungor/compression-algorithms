#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>



int sayac =0;
int tokensayisi =0;
bool mert = false;

struct node{
    uint8_t offset ,length;
    char c;


};
char *dosya_okuma(FILE *f, int *size)
{
    char *content;
    fseek(f, 0, SEEK_END); //imlec dosya sonunda olmus oldu
    *size = ftell(f); //imlecin oldugu konumu bulduk boylelikle boyut bulundu
    content = malloc(*size);
    fseek(f, 0, SEEK_SET); //imlec tekrardan dosya basina alindi
    fread(content, 1, *size, f); //
    *(content+*size)='\0';
    return content; //metnin tamamini dondurmus olduk
}

int esitlik_uzunlugu(char *p1 ,char *p2){
    int max=0;
    char *bekleme;
    bekleme = p2;

    while(p1 < p2 && *p1 == *p2 && p1<bekleme){
        max++;
        p1++;
        p2++;
    }

    return max;
}


///Bu fonksiyonda olusturdugum buyuk cumleyi tokenlerine ayýrdým...
void encode(char cumle[],int cumleBoyut,struct node *t){





    ///Metinin harfleri arasinda gezinmek icin iki adet pointer taimladim...
    char *lookahead ,*search;

    int offset=0;
    int length;
    int geciciofsset;

    int ce = 0;

    ///Buradaki ic ice dongu ile birlikte metinimizin üstünde gezinerek tokenlerine ayirdik...
    for(lookahead=cumle;lookahead<=cumle+cumleBoyut;lookahead++){

            length = 0;
            geciciofsset=0;
            search = lookahead - 255;
            offset = 0;

           ///Buradaki if'ler sayesinde bant genisligimi 250 olarak tutabildim ...
            if(search<cumle){
                search = cumle;
            }



            for(;search<lookahead;search++)
                {

                    geciciofsset++;
                    if(*search == *lookahead){

                        if(length<esitlik_uzunlugu(search,lookahead)){
                            geciciofsset = lookahead - search;
                            offset = geciciofsset;
                            length = esitlik_uzunlugu(search,lookahead);
                        }

                    }

                }

            lookahead += length;
            printf("%d.Token\n",sayac+1);
            printf("offset :%d -> ",offset);
            printf("length :%d -> ",length);
            printf("karakter :%c",*lookahead);
            printf("\n\n");
            t[ce].offset = offset;
            t[ce].length = length;
            t[ce].c = *lookahead;
            sayac++;
            ce++;


    }

}

///Saymak icin kullanilan fonksiyon...
void tokensay(char cumle[],int cumleBoyut){

    struct node *b;
    char *lookahead ,*search;

    int offset=0;
    int length;
    int a;

    for(lookahead=cumle;lookahead<=cumle+cumleBoyut;lookahead++){

            length = 0;
            a=0;
            search = lookahead-255;
            offset = 0;
            if(search<cumle){
                    search = cumle;
            }



            for(;search<lookahead;search++)
                {



                    a++;
                    if(*search == *lookahead){

                        if(length <  esitlik_uzunlugu(search,lookahead)){
                            a = lookahead - search;
                        offset = a;
                        length = esitlik_uzunlugu(search,lookahead);
                        }



                    }

                }lookahead += length;
                tokensayisi ++;

    }

}

struct dugum{
    char harf;
    int frekans;
    struct dugum *left,*right;

};

 struct agac {
    int alan;
    int boyut;
    struct dugum **dugumler;
};

void yer_ayarla(struct agac *tree)
{
    int i,j;
   // int maxFrekans;
    int maxIndex,maxFrekans;
    struct agac *temp;
    for (i =0;i<tree->boyut-1;i++)
    {
        maxIndex=i;
        maxFrekans=tree->dugumler[i]->frekans;

        for (j=i+1;j<tree->boyut;j++)
        {
            if (tree->dugumler[j]->frekans>maxFrekans)
            {
                maxFrekans=tree->dugumler[j]->frekans;
                maxIndex=j;

            }
        }

        if (maxIndex!=i)
        {

            temp=tree->dugumler[maxIndex];
            tree->dugumler[maxIndex]=tree->dugumler[i];
            tree->dugumler[i]=temp;
        }

    }
}

struct dugum *yeniDugum(char harf, int frekans)
{
    struct dugum *temp = malloc(sizeof(struct dugum));
    temp->harf = harf;
    temp->frekans = frekans;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

struct agac *nt()
{
    struct agac *tree = malloc(sizeof(struct agac));
    tree->alan=8;
    tree->boyut=0;
    tree->dugumler=malloc(tree->alan*sizeof(struct dugum ));
    return tree;
}
void *dugumEkle(struct agac *tree, struct dugum *temp)
{
    if (tree->boyut+1==tree->alan)
    {
        tree->alan*= 2;
        tree->dugumler=realloc(tree->dugumler,tree->alan*sizeof(struct dugum ));
    }

    tree->dugumler[tree->boyut]=temp;
    tree->boyut++;

}


struct dugum *dugumSil(struct agac *tree)
{
    tree->boyut--;
    return tree->dugumler[tree->boyut];
}

void ayarla(struct dugum *temp, char *kod, int index,int *sonHali)
{


    int fk;
    int *tut=&sonHali;
    int boyut=*sonHali;


    if (temp->left==NULL&&temp->right==NULL)
    {
        kod[index] = '\0';
        printf("*** %c : %s : %d\n",temp->harf,kod,temp->frekans);
        fk=strlen(kod)*temp->frekans;
        boyut=boyut+fk;
        *sonHali=boyut;



    }
    if (temp->left!=NULL)
    {
        kod[index++] = '0';
        ayarla(temp->left,kod,index,*tut);
        index--;
    }
    if (temp->right!=NULL)
    {
        kod[index++] = '1';
        ayarla(temp->right,kod,index,*tut);
        index--;
    }



}

int main()
{



    FILE *dosya;
    int boyut=0, token_sayisi=0;

    ///Bu sayede dinamik yapıp metinimizi almis olduk...
    char *buyuk_metin ;

    if(dosya=fopen("deneme.txt","rb"))
    {
        buyuk_metin = dosya_okuma(dosya, &boyut);
        fclose(dosya);
    }

    ///Kac adet token olusturacagimizi saymis olduk...
    tokensay(buyuk_metin, boyut);

    ///token saysini bulduktan sonra dinamik bir dizi yaptýk ve bellekten yer ayirdik...
    struct node *array = malloc(tokensayisi*sizeof(struct node));

    ///Metin txt yi tokenlerine ayirmak icin gereklli olanlari fonksiyonumuza gondermis olduk...
    encode(buyuk_metin, boyut,array);


    printf("\n------>Olusturulan token sayisi : %d\n",tokensayisi);
    printf("----->Metin boyutumuz :%d\n",strlen(buyuk_metin));
    printf("------->Sikistirilmis metin boyutumuz :%d",tokensayisi*3);
    printf("\n");


    ///Encoded adinda bir dosya acip icine token bilgilerini attik...

    if(dosya=fopen("lz77.txt", "wb"))
    {
        fwrite(array, sizeof(struct node), tokensayisi, dosya);
        fclose(dosya);
    }

    ///HUFFMAN
    int fileSize=0;
    char *dizi;

    int frekans[255];
    int tokensayisi;
    FILE *f;
    char *isaretci;
    int i;
     if(f=fopen("deneme.txt","rb"))
    {
        dizi = dosya_okuma(f,&fileSize);
        fclose(f);
    }
    tokensayisi=strlen(dizi);
    char* tut = (char*)malloc(sizeof(char) * (tokensayisi * 2 + 1));
    char agaca_yolla[tokensayisi];
    char sayi_tut[tokensayisi];
    int freq[tokensayisi];
    struct agac *tree =nt();
    int b=0,k=0,l=0,o=0,j,m;
    while(o<255){
        frekans[o]=0;
        o++;
    }
    while(tut[b]!='\0'){
        if(b%2==0){
            agaca_yolla[k]=tut[b];
            k++;
        }
        else{
            sayi_tut[l]=tut[b];
            l++;
            }
        b++;
        }
     for(b=0;b<tokensayisi;b++){
         freq[b]=sayi_tut[b];
        }

 /* for(i=0;i<tokensayisi-1;i++){
        for(j=i+1;j<tokensayisi;j++){
            if(agaca_yolla[i]==agaca_yolla[j]){
                freq[i] +=freq[j];
                for(m=j;m<tokensayisi-1;m++){
                    agaca_yolla[m]=agaca_yolla[m+1];
                    freq[m]=freq[m+1];
                }
                tokensayisi--;
            }
        }
    }*/

    isaretci=&dizi[0];
    while (*isaretci!=0)
    {
        frekans[(int)*isaretci]++;
        isaretci++;
    }

    for (i=0;i<255;i++)
    {
        if (frekans[i]>0)
        {
            dugumEkle(tree,yeniDugum(i,frekans[i]));

        }
    }

    while (tree->boyut>1)
    {
        yer_ayarla(tree);
        struct dugum *node =malloc(sizeof(struct dugum));
        struct dugum *rgh = dugumSil(tree);
        struct dugum *lf = dugumSil(tree);
        node->left=lf;
        node->right=rgh;
        int toplam=lf->frekans+rgh->frekans;
        node->frekans=toplam;
        dugumEkle(tree,node);

    }


    char *buffer = malloc(256);



    printf("\n\n ------------HUFFMAN------------\n\n");
    int sonHali=0;
    ayarla(tree->dugumler[0],buffer,0,&sonHali);


    float esasBoyut=(float)fileSize*4*2;

    printf("\n\n***Ana metin boyutu = %d \n",fileSize*4*2);
    printf("***Huffman sonrasi metin boyutu = %d \n",sonHali);


}
