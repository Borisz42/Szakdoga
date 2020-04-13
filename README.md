# Tematika
ELTE IK 2020 Informatikai kar szakdolgozat - Borbély Dávid

Számítógépes grafikában gyakran megjelennek érdekességképpen a fraktálok, de gyakorlati felhasználási módjuk is akad, például tájak generálásban. Az esetek jelentős részében meg szoktak állni a fraktál megjelenítésénél, mert az önmagában is elég kihívást és vizuális élményt jelent, ám a velük való interakció további érdekességeket rejthet magában. 

Szakdolgozatomban fraktálok jelenlétében valósítok meg egyszerű szimulációt, amelyben a valóságot közelítő módon lehet ütközni velük és visszapattanni róluk. Az ütközés megállapításához olyan fraktálokkal dolgozom, melyek kontrakciós leképzések felhasználásával előállíthatóak. Ez esetben a fraktál egy primitívebb alakzat segítségével jön létre, így az alakzat távolság - függvényének és a rajta végrehajtott transzformáció mátrixának ismeretében megállapítható a fraktál távolság-függvénye is.

A megvalósításhoz C++ nyelvet választottam, OpenGL segítségével. Sphere tracing algoritmust és GPU-val gyorsított kirajzolást használok, hogy valós időben tudjam megjeleníteni a számításköltséges fraktálokat.

## Program futtatása

Csomagold ki a C:/ helyre az alpábbi állományt: [OGLPack.zip](cg.elte.hu/~bsc_cg/resources/OGLPack.zip)
majd futtasd a 
```
subst T: C:/
```
parancsot, ezután nyisd meg Visual Studio-val a projektfájlt. A "Local Windows Debugger" gombra kattintva elundul a program fordítása és futtatása. WASD billenytűkkel és nyomva tartott balklikkel lehet mozogni. A szóköz lenyomására elénk kerül a fehér labda, felengedésére kilövell.
