// Program testowy C
int a;	 			// deklaracja jednej zmiennej
float a1, _b, _00;		// deklaracja 3 zmiennych
double PI = 3.1415926;		// deklaracja zmiennej z określoną
				// wartością początkową
unsigned char c;
int fromASCII = 128, toASCII = 255;
int t[10];
struct data {
  int rok;
  int miesiac, dzien;
} d;
void FunkcjaPusta( void )
{
}
int FunkcjaPustaZParametrami( int a, double d )
{
}
float FunkcjaZDeklaracjamiZmiennych( double d )
{ // deklaracje zmiennych
	int a;
	double half = .5;
	int t[7];
	struct data {
	  int rok, miesiac;
	  int dzien;
	} d1;
}
int x1 = fromASCII + 2 * ( 20 +  toASCII ); 
double realTest = 12.34 + .56 + 78.;
void main( void )
{
	int a = 1, b, c, m;
	int t[3];
	struct data {
	  int dzien, miesiac, rok;
	} d;

	FunkcjaPusta(); 
	FunkcjaPustaZParametrami( "x", 123, 12.34 );
	printf( "\n\n\nRozszerzone kody ASCII\n\n" );
	// pętla for
	for ( uc = fromASCII; uc <= toASCII; uc1++ )
	{
		int a;
		int t[2];
		t[0] = 1; t[1] = t[0];
		printf( "%3d:%2c", uc, uc );
		printf(",%d\n",t[1]);
		d.dzien = 1;
	}
	// instrukcja if
	if ( a > 10 )
		b = a;
	if ( a > 1 )
		b = a;
	else
		b = 1;
	if ( a > b )
		if ( a > c )
			m = a;
		else
			m = c;
	else
		if ( b > c )
			m = b;
		else
			m = c;
	while (a > 1)
	        a = a - 2;
	d.rok = 2010;
	do {
	  a++; d.rok++;
	} while (a < 1);
	m = a > b ? (a > c ? a : c) : (b > c ? b : c);
}
