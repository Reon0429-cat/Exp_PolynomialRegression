#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#define WIN_X    800
#define WIN_Y    500

int readCSV();
void draw_callback ();
void compPolynomialRegression (); // 多項式回帰を求める

char *iName = "130001_tokyo_covid19_patients210923.csv";
char *oName = "patients_per_date210923.csv";

int dataNum; // データ数
int patients[1000]; // 新規感染者数
int day[1000]; // 日
 
#define DIMENSION    1 // 多項式の次元
 
double w[DIMENSION+1]; // 学習パラメータ
double estimation; // 予測値

int main(int argc, char **argv)
{
    if (readCSV ()) {
        printf ("ファイルが読み込めません\n");
        return -1;
    }
    compPolynomialRegression ();
    GtkWidget *window;
    GtkWidget *drawing_area;
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "Patients");
    gtk_widget_set_size_request (window, WIN_X, WIN_Y);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    drawing_area = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER(window), drawing_area);
    g_signal_connect (drawing_area, "draw", G_CALLBACK(draw_callback), NULL);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void draw_callback(GtkWidget *widget, cairo_t *cr)
{
    int Origin[] = {50, WIN_Y-50};
    float lineWidth = 1.0;
    int num;
    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
    cairo_set_line_width (cr, lineWidth);
    cairo_move_to (cr, Origin[0], Origin[1]);
    cairo_line_to (cr, Origin[0]+WIN_X-100, Origin[1]);
    cairo_stroke (cr);
    cairo_move_to (cr, Origin[0], Origin[1]);
    cairo_line_to (cr, Origin[0], Origin[1]-WIN_Y+100);
    cairo_stroke (cr);
    cairo_set_font_size (cr, 20.0);
    cairo_move_to       (cr, Origin[0]+WIN_X-100-150, Origin[1]+30);
    cairo_show_text     (cr, "Date");
    cairo_set_font_size (cr, 20.0);
    cairo_move_to       (cr, 20, Origin[1]-WIN_Y+100-20);
    cairo_show_text     (cr, "Patients");
    
    // 新規感染者数を描画
    cairo_set_source_rgba (cr, 0.0, 0.0, 1.0, 1.0);
    cairo_set_line_width (cr, lineWidth);
    for (num = 1; num < dataNum; num++) {
        cairo_move_to (cr, Origin[0]+num*lineWidth, Origin[1]);
        cairo_line_to (cr, Origin[0]+num*lineWidth, Origin[1]-patients[num]/15);
        cairo_stroke (cr);
    }
    
    // 多項式回帰の描画
    for (num = 1; num < dataNum+10; num++) {
        cairo_set_source_rgba (cr, 0.0, 1.0, 0.0, 1.0);
        estimation = 0;
        for (int i = 0; i <= DIMENSION; i++) {
            estimation += w[i] * pow (num, i);
        }
        cairo_move_to (cr, Origin[0]+num*lineWidth, Origin[1]-estimation/15);
        cairo_arc (cr, Origin[0]+num*lineWidth, Origin[1]-estimation/15, 1.0, 0.0, 2.0*3.14);
        cairo_stroke (cr);
    }
}

int readCSV () {
    FILE *fp;
    int cnt;
    int i, j;
    char fbuf[256];
    char iStr[256];
    char seps[]=",";
    char *token;
    char allData[17][40];
    char date[1000][11];
    fp = fopen(iName, "r");
    if(fp == NULL){
        printf("%sファイルが開けません\n", iName);
        return FALSE;
    }
    
    dataNum = 0;
    while (fgets(fbuf,256,fp)) {
        j = 0;
        for (i = 0; i < strlen(fbuf); i++) {
            if (fbuf[i] == ',' && fbuf[i-1] == ',') {
                iStr[j] = ' ';
                j++;
                iStr[j] = fbuf[i];
                j++;
            } else {
                iStr[j] = fbuf[i];
                j++;
            }
        }
        token = strtok(iStr, seps);
        cnt = 0;
        while (token != NULL) {
            sscanf(token, "%s", allData[cnt]);
            token = strtok(NULL, seps);
            cnt ++;
        }
        if (dataNum != 0 && (strcmp (date[dataNum-1], allData[4]) == 0)) {
            patients[dataNum-1] ++;
        } else {
            strcpy (date[dataNum], allData[4]);
            patients[dataNum] = 1;
            day[dataNum] = dataNum;
            dataNum++;
        }
    }
    fclose (fp);
    fp = fopen(oName, "w");
    
    fprintf (fp, "Day,Date,Patients\n");
    
    for (i = 1; i < dataNum; i++) {
        fprintf (fp, "%03d,%s,%d\n", day[i], date[i], patients[i]);
    }
    
    fclose (fp);
    
    return 0;
}

// 多項式回帰を求める
void compPolynomialRegression () {
    
    // 1.行列の要素を求め、代入
      // 1-1.拡大係数行列の正方行列
    int val[DIMENSION+1][DIMENSION+1];
    for (int i=0; i<=DIMENSION; i++) {
        for (int j=0; j<=DIMENSION; j++) {
            double aij = 0;
            for (int k=1; k<dataNum; k++) {
                aij += pow((double)day[k], (double)(i+j));
            }
            val[i][j] = aij;
        }
    }
    
      // 1-2.拡大係数行列の右端の値(学習パラメータ)
    for (int j=0; j<=DIMENSION; j++) {
        double bij = 0;
        for (int k=1; k<=dataNum; k++) {
            double xk = day[k];
            double yk = patients[k];
            bij += pow(xk, (double)(j)) * yk;
        }
        w[j] = bij;
    }
    

    
    // 2.掃き出し法を用いて連立方程式を解く
      // 2-1.対角を1にする
    
    
    
    
      // 2-2.対角以外の列を0にする
    
    
    
}



