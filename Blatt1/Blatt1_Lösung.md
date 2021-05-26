# Blatt 1
## A1
1. Anzahl der Gundungen g = c^a * r
  => #Zustände = 2^g (weil die Prädikate wahr oder falch sein können)
2. Für o = 0 => nur ein Zustand
  
    o = 1 => zwei Zustände
    
    sonst sehr kompliziert => obere Schranke

## A2
1. Nein
2. Ja. Die Operationen aus dem ersten Plan machen keine Bedingungen vom zweiten Plan kaputt => Immer abwechselnd o_i1 und o_i2 => So werden dann beide Ziele g1 und g2 erfüllt

## A3
1. A ist in s anwendbar <=> 

    a) pre ist Teilmenge von s

    b) pre⁺ Teilmenge von s und Schnitt von pre⁻ mit s ist leer
2. gamma(s, A) = (s \ del) U add
3. Pi ist unendlich (z.B. no-op, verändert nichts => beliebige Aneinanderhängung der no-op)
4. S ist endlich
5. Nein. Wenn a_i-1 Vorbedingungen für a_i herstellt, lassen sich a_i und a_i-1 nicht vertauschen
6. Mit quadratischem Aufwand machbar
7. Nein. o1: 0 -> +p1, o2: p1 -> +p2, o3: 0 -> +p1, +p2
    => o1 . o2 ist Lösung, o3 ist optimal aber kein Subplan von o1 . o2
8. Nein
9. 
    a) ja

    b) nein

    c) nein (keine Aussage)

    d) nein (keine Aussage)

    e) keine Aussage