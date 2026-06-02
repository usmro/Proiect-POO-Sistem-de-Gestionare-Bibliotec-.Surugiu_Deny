import sqlite3
import random
import re
import os

data = """
📖 Clasici Românești
Ion de Liviu Rebreanu (1920, Editura Alcalay)
Moromeții (vol. 1) de Marin Preda (1955, ESPLA)
Maitreyi de Mircea Eliade (1933, Editura Cultura Națională)
Enigma Otiliei de George Călinescu (1938, Fundația pentru Literatură și Artă)
Pădurea spânzuraților de Liviu Rebreanu (1922, Cartea Românească)
Ultima noapte de dragoste, întâia noapte de război de Camil Petrescu (1930, Cultura Națională)
Patul lui Procust de Camil Petrescu (1933, Naționala Ciornei)
Cel mai iubit dintre pământeni de Marin Preda (1980, Cartea Românească)
Baltagul de Mihail Sadoveanu (1930, Cartea Românească)
Ciocoii vechi și noi de Nicolae Filimon (1863, Tipografia Națională a lui I. Romanov)
Frații Jderi de Mihail Sadoveanu (1935, Cartea Românească)
Craii de Curtea-Veche de Mateiu Caragiale (1929, Cartea Românească)
Zodia Cancerului de Mihail Sadoveanu (1929, Cartea Românească)
Concert din muzică de Bach de Hortensia Papadat-Bengescu (1927, Editura Ancora)
Mara de Ioan Slavici (1906, Institutul Tipografic Minerva)
Jurnalul fericirii de Nicolae Steinhardt (1991, Editura Dacia)
Orbitor. Aripa stângă de Mircea Cărtărescu (1996, Humanitas)
Nostalgia de Mircea Cărtărescu (1993, Humanitas)
De două mii de ani de Mihail Sebastian (1934, Naționala Ciornei)
Accidentul de Mihail Sebastian (1940, Fundația Regală pentru Literatură și Artă)
🌍 Clasici Internaționali
Mândrie și prejudecată de Jane Austen (1813, T. Egerton)
1984 de George Orwell (1949, Secker & Warburg)
Ferma animalelor de George Orwell (1945, Secker & Warburg)
Marele Gatsby de F. Scott Fitzgerald (1925, Charles Scribner's Sons)
De veghe în lanul de secară de J.D. Salinger (1951, Little, Brown and Company)
Crima și pedeapsa de F.M. Dostoievski (1866, The Russian Messenger)
Frații Karamazov de F.M. Dostoievski (1880, The Russian Messenger)
Război și pace de Lev Tolstoi (1869, The Russian Messenger)
Anna Karenina de Lev Tolstoi (1878, The Russian Messenger)
Un veac de singurătate de Gabriel García Márquez (1967, Editorial Sudamericana)
Dragostea în vremea holerei de Gabriel García Márquez (1985, Oveja Negra)
Mizerabilii de Victor Hugo (1862, A. Lacroix, Verboeckhoven & Cie)
Roșu și negru de Stendhal (1830, A. Levavasseur)
Contele de Monte-Cristo de Alexandre Dumas (1844, Pétion)
Cei trei muschetari de Alexandre Dumas (1844, Baudry)
Procesul de Franz Kafka (1925, Verlag Die Schmiede)
Străinul de Albert Camus (1942, Gallimard)
Ciuma de Albert Camus (1947, Gallimard)
Bătrânul și marea de Ernest Hemingway (1952, Charles Scribner's Sons)
Pe aripile vântului de Margaret Mitchell (1936, Macmillan)
Portretul lui Dorian Gray de Oscar Wilde (1890, Lippincott's Monthly Magazine)
Jane Eyre de Charlotte Brontë (1847, Smith, Elder & Co.)
La răscruce de vânturi de Emily Brontë (1847, Thomas Cautley Newby)
Micile femei de Louisa May Alcott (1868, Roberts Brothers)
Aventurile lui Huckleberry Finn de Mark Twain (1884, Chatto & Windus)
Chemarea străbunilor de Jack London (1903, Macmillan)
Micul prinț de Antoine de Saint-Exupéry (1943, Reynal & Hitchcock)
Moș Goriot de Honoré de Balzac (1835, Werdet)
Doamna Bovary de Gustave Flaubert (1856, Revue de Paris)
Ulise de James Joyce (1922, Sylvia Beach)
🚀 Science Fiction & Fantasy
Dune de Frank Herbert (1965, Chilton Books)
Stăpânul inelelor: Frăția Inelului de J.R.R. Tolkien (1954, George Allen & Unwin)
Hobbitul de J.R.R. Tolkien (1937, George Allen & Unwin)
Harry Potter și Piatra Filozofală de J.K. Rowling (1997, Bloomsbury)
Fundația de Isaac Asimov (1951, Gnome Press)
Eu, robotul de Isaac Asimov (1950, Gnome Press)
Fahrenheit 451 de Ray Bradbury (1953, Ballantine Books)
Cronici marțiene de Ray Bradbury (1950, Doubleday)
Omul din castelul înalt de Philip K. Dick (1962, Putnam)
Visează androizii oi electrice? de Philip K. Dick (1968, Doubleday)
Jocul lui Ender de Orson Scott Card (1985, Tor Books)
Neuromantul de William Gibson (1984, Ace Books)
Solaris de Stanislaw Lem (1961, MON)
Cronicile din Narnia: Leul, vrăjitoarea și dulapul de C.S. Lewis (1950, Geoffrey Bles)
Urzeala tronurilor de George R.R. Martin (1996, Bantam Spectra)
Povestea slujitoarei de Margaret Atwood (1985, McClelland and Stewart)
Problema celor trei corpuri de Liu Cixin (2008, Chongqing Press)
Ghidul autostopistului galactic de Douglas Adams (1979, Pan Books)
Mașina timpului de H.G. Wells (1895, William Heinemann)
Războiul lumilor de H.G. Wells (1898, William Heinemann)
🕵️ Thriller, Mister & Horror
Zece negri mititei de Agatha Christie (1939, Collins Crime Club)
Crima din Orient Express de Agatha Christie (1934, Collins Crime Club)
Câinele din Baskerville de Arthur Conan Doyle (1902, George Newnes)
Tăcerea mieilor de Thomas Harris (1988, St. Martin's Press)
Strălucirea (The Shining) de Stephen King (1977, Doubleday)
It de Stephen King (1986, Viking)
Dracula de Bram Stoker (1897, Archibald Constable and Company)
Frankenstein de Mary Shelley (1818, Lackington, Hughes, Harding, Mavor & Jones)
Fata dispărută de Gillian Flynn (2012, Crown Publishing Group)
Codul lui Da Vinci de Dan Brown (2003, Doubleday)
Îngeri și demoni de Dan Brown (2000, Pocket Books)
Fata cu un dragon tatuat de Stieg Larsson (2005, Norstedts Förlag)
Rebecca de Daphne du Maurier (1938, Victor Gollancz)
Numele trandafirului de Umberto Eco (1980, Bompiani)
Umbra vântului de Carlos Ruiz Zafón (2001, Planeta)
Șoimul maltez de Dashiell Hammett (1930, Alfred A. Knopf)
Misterul camerei galbene de Gaston Leroux (1907, L'Illustration)
Pacientul englez de Michael Ondaatje (1992, McClelland and Stewart)
Un studiu în roșu de Arthur Conan Doyle (1887, Ward Lock & Co)
Colecționarul de John Fowles (1963, Jonathan Cape)
🧠 Non-ficțiune, Filosofie & Psihologie
Sapiens. Scurtă istorie a omenirii de Yuval Noah Harari (2011, Dvir Publishing)
Gândire rapidă, gândire lentă de Daniel Kahneman (2011, Farrar, Straus and Giroux)
Homo Deus de Yuval Noah Harari (2015, Dvir Publishing)
Lebăda neagră de Nassim Nicholas Taleb (2007, Random House)
Scurtă istorie a timpului de Stephen Hawking (1988, Bantam Books)
Jurnalul Annei Frank de Anne Frank (1947, Contact Publishing)
Omul în căutarea sensului vieții de Viktor E. Frankl (1946, Verlag für Jugend und Volk)
Dincolo de bine și de rău de Friedrich Nietzsche (1886, C. G. Naumann)
Așa grăit-a Zarathustra de Friedrich Nietzsche (1883, Ernst Schmeitzner)
Critica rațiunii pure de Immanuel Kant (1781, Johann Friedrich Hartknoch)
Mitul lui Sisif de Albert Camus (1942, Gallimard)
Originea speciilor de Charles Darwin (1859, John Murray)
Cosmos de Carl Sagan (1980, Random House)
Inteligența emoțională de Daniel Goleman (1995, Bantam Books)
Arta războiului de Sun Tzu (Secolul V î.Hr., Tradițional)
Principele de Niccolò Machiavelli (1532, Antonio Blado)
Meditații de Marcus Aurelius (cca. 180 d.Hr., Tradițional)
Interpretarea viselor de Sigmund Freud (1899, Franz Deuticke)
Capitalul de Karl Marx (1867, Verlag von Otto Meissner)
Al doilea sex de Simone de Beauvoir (1949, Gallimard)
📚 Ficțiune Contemporană & Modernă
Hoțul de cărți de Markus Zusak (2005, Picador)
Vânătorii de zmeie de Khaled Hosseini (2003, Riverhead Books)
Splendida cetate a celor o mie de sori de Khaled Hosseini (2007, Riverhead Books)
Să ucizi o pasăre cântătoare de Harper Lee (1960, J.B. Lippincott & Co.)
Ajutorul (The Help) de Kathryn Stockett (2009, Penguin Books)
Istoria secretă de Donna Tartt (1992, Alfred A. Knopf)
Sticletele de Donna Tartt (2013, Little, Brown and Company)
Bula de sticlă de Sylvia Plath (1963, Heinemann)
Jocurile Foamei de Suzanne Collins (2008, Scholastic)
Divergent de Veronica Roth (2011, Katherine Tegen Books)
Sub aceeași stea de John Green (2012, Dutton Books)
Printre tonuri cenușii de Ruta Sepetys (2011, Philomel Books)
Bibliotecara de la Auschwitz de Antonio Iturbe (2012, Planeta)
Tatuatorul de la Auschwitz de Heather Morris (2018, Bonnier Zaffre)
Alchimistul de Paulo Coelho (1988, Rocco)
Băiatul cu pijamale în dungi de John Boyne (2006, David Fickling Books)
Jurnalul unui puști de Jeff Kinney (2007, Amulet Books)
Minunea de R.J. Palacio (2012, Alfred A. Knopf)
Culoarea purpurie de Alice Walker (1982, Harcourt Brace Jovanovich)
Fiica dorinței de Isabel Allende (1999, HarperCollins)
Casa spiritelor de Isabel Allende (1982, Plaza & Janés)
Orbirea de José Saramago (1995, Caminho)
Eseu despre luciditate de José Saramago (2004, Caminho)
Sărbătoarea țapului de Mario Vargas Llosa (2000, Alfaguara)
Orașul și câinii de Mario Vargas Llosa (1963, Seix Barral)
Lolita de Vladimir Nabokov (1955, Olympia Press)
Doctor Jivago de Boris Pasternak (1957, Feltrinelli)
Maestrul și Margareta de Mihail Bulgakov (1967, YMCA Press)
Războiul sfârșitului lumii de Mario Vargas Llosa (1981, Seix Barral)
Zbor deasupra unui cuib de cuci de Ken Kesey (1962, Viking Press)
Abatorul cinci de Kurt Vonnegut (1969, Delacorte Press)
Inima întunericului de Joseph Conrad (1899, Blackwood's Magazine)
Doamna Dalloway de Virginia Woolf (1925, Hogarth Press)
Spre far de Virginia Woolf (1927, Hogarth Press)
Oamenii liberi de Halldór Laxness (1934, E. P. Dutton)
Pădurea norvegiană de Haruki Murakami (1987, Kodansha)
Kafka pe malul mării de Haruki Murakami (2002, Shinchosha)
Femeia la treizeci de ani de Honoré de Balzac (1832, Mame-Delaunay)
Aventurile lui Tom Sawyer de Mark Twain (1876, American Publishing Company)
Colț Alb de Jack London (1906, Macmillan)
⚔️ Aventură & Romane Istorice
Stâlpii pământului de Ken Follett (1989, Macmillan)
Shōgun de James Clavell (1975, Atheneum)
Sinuhe Egipteanul de Mika Waltari (1945, WSOY)
Moby Dick de Herman Melville (1851, Richard Bentley)
Don Quijote de la Mancha de Miguel de Cervantes (1605, Francisco de Robles)
Ivanhoe de Walter Scott (1819, Archibald Constable)
Călătoria spre centrul Pământului de Jules Verne (1864, Pierre-Jules Hetzel)
Douăzeci de mii de leghe sub mări de Jules Verne (1870, Pierre-Jules Hetzel)
Ocolul pământului în 80 de zile de Jules Verne (1872, Pierre-Jules Hetzel)
Insula comorii de Robert Louis Stevenson (1883, Cassell & Co.)
Robinson Crusoe de Daniel Defoe (1719, William Taylor)
Ultimul mohican de James Fenimore Cooper (1826, H.C. Carey & I. Lea)
Călătoriile lui Gulliver de Jonathan Swift (1726, Benjamin Motte)
Ghepardul de Giuseppe Tomasi di Lampedusa (1958, Feltrinelli)
Quo Vadis de Henryk Sienkiewicz (1896, Gazeta Polska)
Eu, Claudius de Robert Graves (1934, Arthur Barker)
Maurice de E.M. Forster (1971, Edward Arnold)
Muntele vrăjit de Thomas Mann (1924, S. Fischer Verlag)
Casa Buddenbrook de Thomas Mann (1901, S. Fischer Verlag)
Lupul de stepă de Hermann Hesse (1927, S. Fischer Verlag)
🎭 Dramaturgie, Poezie & Epopei Antice
Iliada de Homer (Secolul VIII î.Hr., Tradiție antică)
Odiseea de Homer (Secolul VIII î.Hr., Tradiție antică)
Eneida de Vergilius (19 î.Hr., Tradiție antică)
Divina Comedie de Dante Alighieri (1320, Manuscris)
Romeo și Julieta de William Shakespeare (1597, John Danter)
Hamlet de William Shakespeare (1603, Nicholas Ling & John Trundell)
Macbeth de William Shakespeare (1623, Edward Blount & William Jaggard)
Faust de Johann Wolfgang von Goethe (1808, J.G. Cotta'sche Buchhandlung)
O scrisoare pierdută de I.L. Caragiale (1884, Socec)
Steaua fără nume de Mihail Sebastian (1944, Fundația Regală)
Luceafărul de Mihai Eminescu (1883, Almanahul Societății Academice)
Plumb de George Bacovia (1916, Flacăra)
Cuvinte potrivite de Tudor Arghezi (1927, Cartea Românească)
Poemele luminii de Lucian Blaga (1919, Cosânzeana)
Așteptându-l pe Godot de Samuel Beckett (1952, Les Éditions de Minuit)
Pescărușul de Anton Cehov (1896, Russkaya Mysl)
Livada de vișini de Anton Cehov (1904, Znanie)
Cyrano de Bergerac de Edmond Rostand (1897, Charpentier et Fasquelle)
Antigona de Sofocle (441 î.Hr., Tradiție antică)
Oedip Rege de Sofocle (429 î.Hr., Tradiție antică)
🌟 Bestselleruri & Literatură Contemporană
Femeia de la fereastră de A.J. Finn (2018, William Morrow)
Acolo unde cântă racii de Delia Owens (2018, G.P. Putnam's Sons)
Oameni normali de Sally Rooney (2018, Faber & Faber)
Conversații cu prieteni de Sally Rooney (2017, Faber & Faber)
Soția călătorului în timp de Audrey Niffenegger (2003, MacAdam/Cage)
Prietena mea genială de Elena Ferrante (2011, Edizioni e/o)
Povestea noului nume de Elena Ferrante (2012, Edizioni e/o)
Zuleiha deschide ochii de Guzel Iahina (2015, AST)
Fight Club de Chuck Palahniuk (1996, W. W. Norton & Company)
O viață măruntă de Hanya Yanagihara (2015, Doubleday)
Oscar și Tanti Roz de Eric-Emmanuel Schmitt (2002, Albin Michel)
Eleganța ariciului de Muriel Barbery (2006, Gallimard)
Mănâncă, roagă-te, iubește de Elizabeth Gilbert (2006, Viking Press)
Sălbăticie (Wild) de Cheryl Strayed (2012, Alfred A. Knopf)
Trei surori de Heather Morris (2021, Bonnier Books)
Zborul de noapte de Antoine de Saint-Exupéry (1931, Gallimard)
Pianistul de Władysław Szpilman (1946, Wiedza)
Băiatul care a îmblânzit vântul de William Kamkwamba (2009, William Morrow)
Gemenii de Mircea Cărtărescu (1996, Humanitas)
🛸 Distopii & Universuri Fantastice (Partea a II-a)
Minunata lume nouă de Aldous Huxley (1932, Chatto & Windus)
Portocala mecanică de Anthony Burgess (1962, William Heinemann)
Fundația și Imperiul de Isaac Asimov (1952, Gnome Press)
A doua Fundație de Isaac Asimov (1953, Gnome Press)
Copiii Dunei de Frank Herbert (1976, Berkley Books)
Mântuitorul Dunei de Frank Herbert (1969, Galaxy Science Fiction)
Cele două turnuri de J.R.R. Tolkien (1954, George Allen & Unwin)
Întoarcerea regelui de J.R.R. Tolkien (1955, George Allen & Unwin)
Harry Potter și Camera Secretelor de J.K. Rowling (1998, Bloomsbury)
Harry Potter și Prizonierul din Azkaban de J.K. Rowling (1999, Bloomsbury)
Numele vântului de Patrick Rothfuss (2007, DAW Books)
Calea regilor de Brandon Sanderson (2010, Tor Books)
Ultimul imperiu (Mistborn) de Brandon Sanderson (2006, Tor Books)
Corabia magiei de Robin Hobb (1998, Bantam Spectra)
Asasinul regal de Robin Hobb (1995, Bantam Spectra)
Pădurea întunecată (Seria Amintiri din trecutul Pământului) de Liu Cixin (2008, Chongqing Press)
Capătul morții de Liu Cixin (2010, Chongqing Press)
V de la Vendetta de Alan Moore & David Lloyd (1982, Quality Communications)
Watchmen de Alan Moore & Dave Gibbons (1986, DC Comics)
Sandman: Preludii și nocturne de Neil Gaiman (1989, DC Comics)
📈 Dezvoltare Personală, Business & Biografii
Atomic Habits de James Clear (2018, Avery)
Tată bogat, tată sărac de Robert T. Kiyosaki (1997, Warner Books)
De la idee la bani de Napoleon Hill (1937, The Ralston Society)
Cum să câștigi prieteni și să influențezi oameni de Dale Carnegie (1936, Simon & Schuster)
Cele 7 deprinderi ale persoanelor extrem de eficace de Stephen R. Covey (1989, Free Press)
Arta subtilă a nepăsării de Mark Manson (2016, HarperOne)
Esențialismul de Greg McKeown (2014, Crown Business)
Blink: Decizii bune în 2 secunde de Malcolm Gladwell (2005, Little, Brown and Co.)
Excepționalii (Outliers) de Malcolm Gladwell (2008, Little, Brown and Co.)
Începe cu de ce de Simon Sinek (2009, Portfolio)
Povestea mea (Becoming) de Michelle Obama (2018, Crown Publishing)
Pământul făgăduinței de Barack Obama (2020, Crown Publishing)
Steve Jobs de Walter Isaacson (2011, Simon & Schuster)
Jurnalul unui mag de Paulo Coelho (1987, Rocco)
Puterea prezentului de Eckhart Tolle (1997, Namaste Publishing)
Regula 10X de Grant Cardone (2011, John Wiley & Sons)
Omul cel mai bogat din Babilon de George S. Clason (1926, Financial Education)
Secretele minții de milionar de T. Harv Eker (2005, HarperCollins)
Gândește fix pe dos de Paul Arden (2006, Penguin Books)
Drive: Ce anume ne motivează cu adevărat de Daniel H. Pink (2009, Riverhead Books)
"""

conn = sqlite3.connect('biblioteca.db')
c = conn.cursor()

# Ensure Location columns exist
columns = [row[1] for row in c.execute("PRAGMA table_info(Carti)").fetchall()]
if 'cladire' not in columns:
    c.execute("ALTER TABLE Carti ADD COLUMN cladire TEXT")
if 'camera' not in columns:
    c.execute("ALTER TABLE Carti ADD COLUMN camera TEXT")
if 'culoar' not in columns:
    c.execute("ALTER TABLE Carti ADD COLUMN culoar TEXT")
if 'raft' not in columns:
    c.execute("ALTER TABLE Carti ADD COLUMN raft TEXT")

# Delete old data to make room for the new list
c.execute("DELETE FROM Carti")
c.execute("DELETE FROM Imprumuturi")
c.execute("DELETE FROM IstoricImprumuturi")

lines = data.split('\n')
current_cat = "General"

for line in lines:
    line = line.strip()
    if not line: continue
    
    # Check if category
    if any(line.startswith(x) for x in ['📖', '🌍', '🚀', '🕵', '🧠', '📚', '⚔', '🎭', '🌟', '🛸', '📈']):
        current_cat = line.split(" ", 1)[1].strip()
        continue
        
    m = re.match(r'^(.*) de (.*?) \((.*?),\s*(.*?)\)$', line)
    if not m:
        # Fallback if no exact match (e.g. some emojis weirdness)
        continue
        
    titlu = m.group(1).strip()
    autor = m.group(2).strip()
    an_str = m.group(3).strip()
    editura = m.group(4).strip()
    
    an = 2000
    if an_str.isdigit():
        an = int(an_str)
    elif "î.Hr." in an_str:
        # random negative for ancient
        an = -800
        
    # Generate random ISBN
    isbn = f"978-973-{random.randint(100, 999)}-{random.randint(100, 999)}"
    
    # Prices and pages
    pret = float(random.randint(20, 150))
    pagini = random.randint(100, 1200)
    
    # Format choice (0: Fizica 60%, 1: Digitala 20%, 2: Audio 20%)
    fmt = random.choices([0, 1, 2], weights=[0.6, 0.2, 0.2])[0]
    
    stoc = 0
    stoc_defect = 0
    link = ""
    dim = 0
    durata = 0
    narator = ""
    cladire = ""
    camera = ""
    culoar = ""
    raft = ""
    
    if fmt == 0:
        stoc = random.randint(2, 20)
        stoc_defect = random.choices([0, 1, 2], weights=[0.8, 0.15, 0.05])[0]
        cladire = random.choice(["Centrala", "Filiala N", "Filiala S"])
        camera = f"Sala {random.randint(1, 5)}"
        culoar = f"Culoar {random.choice(['A', 'B', 'C'])}"
        raft = f"Raft {random.randint(1, 10)}"
    elif fmt == 1:
        stoc = 999
        link = f"https://biblioteca.ro/pdf/{random.randint(1000, 9999)}.pdf"
        dim = random.randint(1000, 50000)
    elif fmt == 2:
        stoc = 999
        link = f"https://biblioteca.ro/audio/{random.randint(1000, 9999)}.mp3"
        durata = random.randint(120, 900)
        narator = random.choice(["Marcel Iures", "Victor Rebengiuc", "Marius Manole", "Oana Pellea", "Andrei Plesu", "Sintetic AI"])
        
    data_adg = "01/06/2026"
    contor = random.randint(0, 100)
    
    c.execute('''
        INSERT INTO Carti 
        (isbn, titlu, autori, editura, pret_intrare, categorie, an_aparitie, nr_pagini, 
         stoc_disponibil, stoc_defect, data_adaugarii, contor_imprumuturi, tip_format, 
         stare_defecta, link_acces, dimensiune_kb, is_deleted, durata_minute, narator,
         cladire, camera, culoar, raft)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, ?, ?, 0, ?, ?, ?, ?, ?, ?)
    ''', (isbn, titlu, autor, editura, pret, current_cat, an, pagini,
          stoc, stoc_defect, data_adg, contor, fmt, link, dim, durata, narator,
          cladire, camera, culoar, raft))

conn.commit()
conn.close()

# Also delete db_carti.txt so the C++ app will dump the new SQLite data on next run
if os.path.exists("db_carti.txt"):
    os.remove("db_carti.txt")
if os.path.exists("db_imprumuturi.txt"):
    os.remove("db_imprumuturi.txt")
if os.path.exists("db_istoric.txt"):
    os.remove("db_istoric.txt")

print(f"Baza de date a fost populata cu succes!")
