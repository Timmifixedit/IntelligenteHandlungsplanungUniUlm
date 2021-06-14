# Blatt3

## Aufgabe 1
* S1 bedroht S3
* zu S1 gibt es keinen Kausalen Link
* S2 Fehlt ebenfalls ein Kausaler Link

## Aufgabe 2
1. SNLP
    * Keine Planschritte => keine Inkonsistenz
    * Vollständig? => Nein
    * Keine Kausalen Beziehungen
    * 4b: move mit x = A, z = B => P = {move(A, y, B)}
    
    ...

Kann der Algo auch suboptimale Lösungen finden: Ja, füge Aktionen ein, die sich gegenseitig die Vorbedingungen kaputt machen (immer abwechselnd)
Um das zu verhindern, könnte man iterative deepening o.Ä. verwenden => Wahl einer geeigneten Suchstrategie

2. Ja, er findet die Lösung. Da Breitensuche wird ein möglichst flacher Plan gefunden.
3. 