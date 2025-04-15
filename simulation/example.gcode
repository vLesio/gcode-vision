; Start
G21             ; Używaj jednostek metrycznych (mm)
G90             ; Pozycjonowanie absolutne
M82             ; Ekstruzja absolutna
G28             ; Powrót do pozycji bazowej (homing wszystkich osi)
G92 E0          ; Zresetuj licznik ekstruzji
G1 Z0.2 F600    ; Podnieś głowicę do wysokości 0.2mm

; Rysowanie kwadratu 20x20mm
G1 X0 Y0 F1500       ; Przejdź do punktu startowego
G1 X20 Y0 E1.0 F1800 ; Dolna krawędź
G1 X20 Y20 E2.0      ; Prawa krawędź
G1 X0 Y20 E3.0       ; Górna krawędź
G1 X0 Y0 E4.0        ; Lewa krawędź

; Koniec
G1 Z10 F1000         ; Podnieś głowicę
G92 E0               ; Zresetuj licznik ekstruzji
M104 S0              ; Wyłącz grzanie hotendu
M140 S0              ; Wyłącz grzanie stołu
M84                  ; Wyłącz silniki
