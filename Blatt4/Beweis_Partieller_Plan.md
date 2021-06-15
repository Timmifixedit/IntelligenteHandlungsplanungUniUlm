# Satz
Ein partieller Plan $\pi = (A, <, B, L)$ ist Lösung eines Problems $(O, s_o, g)$ falls $<, B$ konsistent sind und $\pi$ keine Flaws enthält. D.h. Alle Linearisierungen (totale Ordnungen der Aktionen) sind ausführbar und erreichen $g$

### Beweis durch Induktion über Anzahl der Aktionen in $A$
#### **Induktionsanfang**
$A = \{a_0, a_1, a_\infty\}$ mit $a_0$ Startaktion, $a_\infty$ Zielaktion.
Hier gibt es nur eine Sequenz an total geordneten Aktionen. Diese Aktion ist ausführbar, weil der Plan keine Flaws enthält, da alle Preconditions durch kausale Links gestützt werden.
#### **Induktionshypothese**
Die Proposition gelte für alle Pläne mit $n-1$ Aktionen

#### **Induktionsschritt**
Sei $\pi$ Plan mit $n$ Aktionen ohne Flaws
Sei $A_i = \{a_{i, 1}, ..., a_{i, n}\} \subseteq A$ Menge von Aktionen ohne Vorgänger bezüglich $<$ außer $a_0$

Jede total geordnete Aktionsfolge der Aktionen in $\pi$ muss mit einer dieser Aktionen $a_i \in A_i$ beginnen.

Da $\pi$ keine Flaws enthält, sind alle Preconditions von $a_i$ durch kausale Links gestützt

$\Rightarrow a_i$ anwendbar in $s_0$ 

Sei $s'$ der Zustand, den wir erhalten, wenn wir $a_i$ ins $s_0$ ausführen und sei $\pi' = (A', <', B', L')$ der verbleibende Plan, welcher aus $\pi$ durch Ersetzen der Aktion $a_0$ und $a_i$ durch eine neue Aktion entstanden ist, die $s'$ repräsentiert. (Also wende $a_i$ ind $a_0$ an. Dann ist $s'$ der neue Startzustand von $\pi'$). Die ganzen Ordnungsrelationen etc. werden beibehalten.

Betrachte $\pi'$:

* $<'$ ist nach wie vor konsistent, da wir keine neuen Ordnungen hinzugefügt haben.
* Ebenso $B'$, da wir keine neuen Constraints hinzugefügt haben.
* $\pi'$ hat keine Threats, da keine Aktion hinzugefügt wurde
* Alle Preconditions sind durch kausale Links gestützt

$\Rightarrow \pi'$ hat $n - 1$ Aktionen und keine Flaws. Aus der Induktionshypothese folgt der Beweis

#### **Achtung**
Die Proposition ist eine Implikation. Die Rückrichtung gilt im Allgemeinen nicht!