# Changelog

Alle wesentlichen Änderungen an diesem Projekt werden in dieser Datei
dokumentiert. Die Versionsnummern folgen der semantischen Versionierung.

## [1.0.0] – 2026-10-05

Erste öffentliche Fassung:

- SR, RS, P_TRIG und N_TRIG
- Optional verlängerbare P_TRIG- und N_TRIG-Ausgänge über `activeCycles`
- `reset()` bricht eine Flankenverlängerung ab, ohne eine künstliche Folgeflanke
  durch Zurücksetzen des gespeicherten Eingangs zu erzeugen
- TON, TOF und TP mit nichtblockierender `millis()`-Zeitbasis
- CTU, CTD und CTUD mit `int32_t`-Zählwerten
- FirstScan, CycleTime und ClockMemory
- Zeithelfer T_MS, T_SEC, T_MIN und T_HOUR
- Sechs verständliche Beispiel-Sketches für Arduino und ESP32
- Ausführliche deutschsprachige README mit API- und Installationshinweisen
- Unlicense und transparenter Hinweis zu KI-generierten Inhalten
