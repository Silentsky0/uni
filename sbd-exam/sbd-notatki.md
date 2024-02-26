# Struktury Baz Danych - notatki na egzamin

## Podstawowe wzory i własności

- Ziarnistość - wielkość strony dyskowej (typowo 4kB)

- **blocking factor** $ b = B/R $, gdzie:
    - $B$ - wielkość strony dyskowej
    - $R$ - średnia długość rekordu

    blocking factor oznacza średnią liczbę rekordów na jednej stronie lub
    średnią liczbę rekordów odczytanych/zapisanych w jednej operacji dyskowej

- oznaczenia symboli
    - $N$ - liczba rekordów
    - $R$ - średnia długość(rozmiar) rekordu
    - $b$ - blocking factor
    - $r$ - liczba serii w pliku sekwencyjnym
    - $d$ - rząd drzewa (B-drzewa)
    - $h$ - wysokość drzewa (root jest na poziomie $h=1$)

> Wzory, w których występuje logarytm zakładają logarytm binarny

## Rodzaje plików i ich właściwości

### Pliki seryjne

- Rozmiar pliku - $N/b$ stron dyskowych
- Średni czas czytania całego pliku - $N/b$
- Średni czas szukania jednego rekordu - $\frac{1}{2} N/b$
- Wstawianie
  - bez cachowania - jeden lub dwa dostępy
  - z cachowaniem - średnio $1/b$ dostępów
- Aktualizacja rekordu - praktycznie niemożliwa
- Spanning - dzielenie rekordów na części
  - Całkowicie wypełnia strony dyskowe
  - Kosztowniejszy odczyt i zapis

### Pliki sekwencyjne

- Szukanie sekwencyjne - $\frac{1}{2} N/b$
- Szukanie binarne - $\log (N/b)$
- Szukanie interpolacyjne (sampling) - $\log_2\log_2 (N/b)$
- Wstawienie - $O(N)$, do overflow area
- Reorganizacja - $O((N+V) \log(N+V))$, posortowanie całego pliku 
  - Sortowanie overflow - $O(V\log V)$
  - Scalanie - $2(N + V) / b$
- Otrzymanie kolejnego rekordu - $1/b$

Sortowanie metodą scalania naturalnego (strategia 2+1)
- Liczba faz - $\lceil \log r \rceil$
- Liczba dostępów - najgorszy przypadek - $4N \lceil \log N \rceil /b$
- Liczba dostępów - przypadek średni - $4N \lceil \log r \rceil /b$
> Dla strategii 2+2 wartości dostępów są dwa razy mniejsze: $2N \lceil \log N \rceil /b$

Sortowanie metodą scalania polifazowego
- Liczba faz - $1.45 \log r$
- Ale każdy rekord jest odczytywany/zapisywany średnio $1.04 \log r$
- Średnia liczba operacji dyskowych - $2N (1.04 \log r + 1) / b$

Sortowanie wielkimi buforami - strukturą w pamięci jest stóg
- Liczba dostępów - $2 \frac{N}{b \log n} \lceil \log N/b \rceil$

### Pliki indeksowo-sekwencyjne

> indeks rzadki - indeks zawiera tylko odsyłacze do pierwszego rekordu z każdej
  strony

Czas dostępu do rekordu - $\log SI_N + 1 + V/(2N)$, gdzie $SI_N$ to rozmiar
indeksu, a $V$ overflow area

Koszt reorganizacji - czas czytania całego pliku + czas przeczytania nowej
strony + czas przeczytania całego indeksu

### Pliki indeksowe - B-drzewa

- Minimalna liczba kluczy - $2((d + 1)^{h-1} - 1)$
- Maksymalna liczba kluczy - $(2d + 1)^h - 1$

Czas szukania klucza - $\log_d N$

Typowa zajętość pamięci B-drzewa to $\approx 70\%$ ($\ln 2$)

### Pliki indeksowe - B+drzewa

$d^*$ - rząd węzłów, które nie są liścmi
$r$ - rząd liści

- Minimalna liczba kluczy - $N_{min} = 2r(d^* + 1)^{h^* - 2}$
- Maksymalna liczba kluczy - $N_{max} = 2r(2d^* + 1){h^* - 1}$

### Pliki rozproszone

Efekt klastrowania rekordów jest korzystny z uwagi na odczyty blokowe -
najkorzystniejszą strategią rozwiązywania kolizji jest adresowanie otwarte z
sondowaniem liniowym

W rozpraszaniu rozszerzalnym, potrzebne są jedynie 1 lub 2 dostępy - 2 jeżeli
skorowidz nie mieści się w pamięci operacyjnej
