
Die Übersetzungen der englischen Texte des PhpWikis in Deutsche sind
derzeit und schon länger nicht mehr aktuell und auf dem Laufenden. Das
ist vor allem dadurch bedingt, dass die englischen Texte beständig
ergänzt, überholt und verändert wurden. Übrigens sind auch die anderen
fremdsprachigen Texte gegenüber dem englischen Original nicht mehr
ganz zeitgemäss. Die PhpWiki Entwickler freuen sich immer, wenn von
anderen auch an er sprachlichen Aktualisierung von PhpWiki
weitergefeilt wird.

Für diejenigen, die daran mitwirken wollen, sind hier ein paar
sachdienliche Vorschläge: Bitte laden Sie sich zunächst die neueste
PhpWiki Version auf Ihren Rechner von:

Nightly CVS snapshot:
http://phpwiki.sf.net/nightly/phpwiki.nightly.tar.gz

Beachten Sie bei der weiteren Bearbeitung bitte, dass Sie die Datei:
phpwiki/locale/de/LC_MESSAGES/phpwiki.php direkt nicht editieren
sollten, sondern vielmehr folgende: phpwiki/locale/po/de.po. Achten
Sie auch darauf, diese Datei mit ISO-8859-1 abzuspeichern (nicht mit
Macintosh Charset oder Windows-1252), sonst fehlen die Umlaute und das
nach älterer Schreibweise noch gebräuchliche scharfe S bzw. Sz "ß".

Zunächst wurden die Dateien in LC_MESSAGES automatisch erstellt:
phpwiki.mo u. phpwiki.php. Das braucht aber GNUmake: (Seite auf
Englisch) http://www.gnu.org/software/make/make.html

cd phpwiki/locale
make

Wenn Sie Windows benutzen, d.h. nicht Linux noch Unix/Mac OS X, können
Sie auch GNUmake installieren. Eine Installationbeschreibung hierfür
an dieser Stelle zu liefern wäre zu ausführlich. In diesem Falle ist
es für beide Seiten, für Sie wie für mich, vom Aufwand her vermutlich
einfacher und effektiver, wenn Sie mir Ihre neue
phpwiki/locale/po/de.po Datei einfach schicken, und ich sie dann
umsetzen kann. Sie erhalten danach phpwiki.mo und phpwiki.php per Mail
zurück (was mir ein Freude wäre!)

Alle Ihre Korrektur werden dann von mir, sobald ein entsprechender
Umfang erreicht ist, bei geeigneter Gelegenheit in ihrer Gesamtheit
ins PhpWiki-CVS (auf Sourceforge) eingesetzt und damit dann für alle
PhpWiki Benutzer zugänglich und wirksam.

--CarstenKlapp <carstenklapp@users.sourceforge.net>
