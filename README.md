# Tematika
ELTE IK 2020 Informatikai kar szakdolgozat - Borbély Dávid

Számítógépes grafikában gyakran megjelennek érdekességképpen a fraktálok, de gyakorlati felhasználási módjuk is akad, például tájak generálásban. Az esetek jelentős részében meg szoktak állni a fraktál megjelenítésénél, mert az önmagában is elég kihívást és vizuális élményt jelent, ám a velük való interakció további érdekességeket rejthet magában. 

Szakdolgozatomban fraktálok jelenlétében valósítok meg egyszerű szimulációt, amelyben a valóságot közelítő módon lehet ütközni velük és visszapattanni róluk. Az ütközés megállapításához olyan fraktálokkal dolgozom, melyek kontrakciós leképzések felhasználásával előállíthatóak. Ez esetben a fraktál egy primitívebb alakzat segítségével jön létre, így az alakzat távolság - függvényének és a rajta végrehajtott transzformáció mátrixának ismeretében megállapítható a fraktál távolság-függvénye is.

A megvalósításhoz C++ nyelvet választottam, OpenGL segítségével. Sphere tracing algoritmust és GPU-val gyorsított kirajzolást használok, hogy valós időben tudjam megjeleníteni a számításköltséges fraktálokat.

## Program futtatása

A futtatáshoz szükséges legalább Windows 7 opreációs rendszer és OpenGL 3.0 támogatottság, vagyis nagyjából minden 2009 utáni számítógép alkalmas a futtatásra. Az optimális élményhez elengedhetetlen a dedikált viedókártya.

Az alkalmazás elindításához a **FractalCollision.exe** fájlt kell futtatni. A program sok csomagot használ ami nem feltétlen található meg minden számítógépen, így a **Release** mappában megtalálható minden ami a működéséhez szükséges, így abból a mappából indítva biztosan működni fog. 

Az alkalmazásból való kilépéshez lehet az **ESC** billentyűt vagy a jobb felső sarokban az ablak bezárás gombját használni. Ha bezárjuk a terminálablakot akkor mindkét ablak bezárul, ha először a fő programablakot zárjuk be akkor utána még külön be kell zárni a terminálablakot.

Az alkalmazás **Microsoft Visual Studio** segítségével készült, így ha újra akarnánk fordítani akkor a **C:/** helyre csomagoljuk ki a mellékelt **OGLPack.zip** állományt (ez az OpenGL-hez szükséges fájlokat tartalmazza), majd futtassuk a **subst T: C:/** parancsot. Ezután megnyithatjuk a **.vcxproj** projektfájlt.
