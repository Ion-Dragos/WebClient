# Web Client
### Ion Dragos - Cosmin

In fisierul request.c am modificat functiile de GET si POST conform enuntului(am adaugat si header-ul pentru tokenul JWT) si am adaugat functia de DELETE care e asemanatoare cu cele 2.

Ca JSON parser am folosit parserul https://github.com/kgabis/parson, cel indicat si in enunt de altfel. L-am folosit pe acesta pentru ca:
1. voiam sa folosesc o librarie, nu sa scriu eu una.
2. dupa ce m-am uitat putin pe documentatie mi s-a parut super intuitiv si explica fix de ce aveam nevoie. Librarie este intr-un singur header file.

**register**
    Trimitem o cerere POST catre server cu username-ul si parola citite de la tastatura. Daca username-ul este deja folosit se da un mesaj de eroare. In caz contrar se afiseaza un mesaj de succes. 

**login**
    La login e aceeasi idee. Se citeste de la tastatura si se verifica username-ul sau parola sunt gresite si se trateaza cazurile cu mesaje de eroare. Daca am avut succes o sa pastram cookie-ul parsat intr-un pointer de tip char.  

**enter_library**
    Clientul trimite o cerere de tip GET si dovedeste ca este autentificat prin cookie-ul primit mai devreme in cazul in care nu suntem logati afisam mesajul de eroare, in cazul in care primim acces la bibloteca primim un token JWT si-l salvam.

**get_books**
    Aici trimitem o cerere GET cu token-ul obtinut pentru acces la biblioteca. Daca nu avem acces afisam un mesaj de eroare, iar in cazul in care avem acces afisam doar lista cu cartile, expunand doar id-ul si titlul.

**get_book**
    Trimitem o cerere GET cu token-ul obtinut anterior. daca nu il avem afisam un mesaj ca nu avem acces. Daca avem acces citim de la tastatura id-ul pentru care carte dorim sa aflam informatii. La finalul url-ului, adaugam cu ajutorul sprintf id-ul. In cazul in care nu exista aceasta carte afisam ca nu exista aceasta carte, iar in cazul in care exista, afisam informatiile de la server despre carte doar ca adaugam la inceput si id-ul cartii.

**add_book**
    Datele pe care le citim de la tastatura pentru a adauga o carte noua le salvam intr-un JSON. In cazul in care introducem o carte cu cumar de pagini 0 sau negativ nu se adauga cartea. 

**delete_book**
    Clientul trimite o cerere de tip DELETE spunand serverului sa stearga toate informatiile despre o anumita carte. Citim id-ul si il punem la finalul url-ului. Daca nu exista cartea afisam un mesaj de eroare, in caz contrat afisam ca s-a sters cartea cu succes.

**logout**
    Verificam daca am fost autentificati mai intai si se da logout, daca nu,apare un mesaj de eroare ca nu am intrat in cont. 

**exit**
    Inchidem conexiunea si afisam un mesaj personalizat, iesind din while.

In cazul in care introducem o comanda gresita, afisam un mesaj ca am tastat gresit comanda si sa mai incercam. 