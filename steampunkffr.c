/*
 * =====================================================================================
 *
 *       Filename:  steampunkffr.c
 *
 *    Description:  Steampunk FFR - How it started
 *    				A "This-is-youre-Adventure" novel
 *
 *        Version:  0.008
 *    letzte Beta:  0.000
 *        Created:  22.05.2011 09:35:00
 *          Ended:  00.00.0000 00:00:00
 *       Revision:  none
 *       Compiler:  clang
 *        compile:  clang -o steampunkffr steampunkffr.c -lncurses
 *
 *         Author:  Sascha K. Biermanns (saschakb), saschakb@gmail.com
 *        Company:
 *        License:  ISC
 *
 *   Copyright (C)  2011, Sascha K. Biermanns
 *
 * ====================================================================================
 *
 *   Permission to use, copy, modify, and/or distribute this software for any
 *   purpose with or without fee is hereby granted, provided that the above
 *   copyright notice and this permission notice appear in all copies.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * =====================================================================================
 *
 *   Last changes:
 *   - 22.05.2011 Beginn an der Arbeit des Spiels
 *   - 22.05.2011 Schusswaffen und Munition implementiert
 *   - 22.05.2011 Tripodenauftauchen eingebaut
 *   - 12.06.2011 Changed the engine to the english language
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#define FILENAME ".steampunkffrsicherung.txt"

// Neuer Typ: character_t & npc_t
// Zur Speicherung der Kampfwerte

typedef struct _character {
	char name[30];
	int dexterity;
	int dexterity_start;
	int strength;
	int strength_start;
	int luck;
	int luck_start;
} character_t;

// -----------
// Der player
// -----------
static character_t player;

// ----------------------
// Variablen des Spielers
// ----------------------

enum objectsynonyms { nichts, astgabel, bogensilberpfeil, buch, drachenfeuerzauber, dexteritypotion, riffle, luckpotion, goldstuecke, handschuh, helm, holzhammer, holzpflock, holzschild, juwelenauge, kaese, kerze, laterne, lederruestung, lederruestung2, ohrringe, ammunition, pergament, gun, proviant, rucksack, schild, schwert, schwert2, silberkreuz, silbersichel, strengthpotion, taschenlampe, taschenmesser, unsichtbarkeitstrank, verbandskasten, verwuenschterhelm, warndreieck, wunderwasser, zombiegold ,maximumobject };

static int object[maximumobject] = { [laterne] = 1, [lederruestung] = 1, [proviant] = 10, [rucksack] = 1, [schwert] = 1 };
static char *objectname[maximumobject] = { "Nichts", "Astgabel", "Bogen und Silberpfeil", "Buch", "Drachenfeuerzauber", "dexteritypotion", "riffle", "Glückstrank", "Goldstücke", "Handschuh", "Helm", "Holzhammer", "Holzpflock", "Holzschild", "Juwelenauge", "Käse", "blaue Kerze", "Laterne", "Lederrüstung", "Lederrüstung", "Ohrringe", "Patronen", "Pergament", "gun", "Proviant", "Rucksack", "Schild", "Schwert", "Zauberschwert +2", "Silberkreuz", "Silbersichel", "Stärketrank", "Pumptaschenlampe", "Taschenmesser", "Unsichtbarkeitstrank", "Verbandskasten", "Verwuenschter Helm", "Warndreieck",  "Wunderwasser", "Zombiegold" };

static int attackbonus = 0;
static int fuenfwahl = 0;
static bool onlysilverhits = false;
static int paralized = 0;
static int price = 0;
static int schluessel = 0;
static bool schluessel9 = false;
static bool schluessel66 = false;
static bool schluessel99 = false;
static bool schluessel111_1 = false;
static bool schluessel111_2 = false;
static bool schluessel125 = false;
static bool schluesselbootshaus = false;
static bool cheating = false;
static bool silverweapon = false;
static bool invisible = false;
static bool tripodgesehen = false;
static int killedpeople = 0;
static int killedfoes = 0;
static bool agartha = false;
static bool verzeichnisgelesen = false;
static bool buchgefunden = false;
static bool kartegefunden = false;
static bool sargverschoben = false;
static bool durchganggeoeffnet = false;
static bool schluesselgefunden = false;

// -------------
// room Variable
// -------------

static int room = 0;

// ----------------
// quitting Variable
// ----------------

bool quitting = false;

// -----------------------------------------
// Spezialmonster (immun außer gegen Silber)
// -----------------------------------------

character_t Schuppiges_scheusal = { "Schuppiges Scheusal", 9, 9, 6, 6 };

// -------------------
// Funktionsprototypen
// -------------------

// Funktion: dice
int dice(unsigned int);

// Funktion: Test your luck
bool tyl(character_t *);

// Funktion: fightround
bool fightround(character_t *, character_t *, void (*)());

// Funktion: fight
bool fight(character_t *, character_t *, int, bool, void (*)());

// Funktion: Momentane Werte
void actual_values(character_t *);

// Funktion: key
char key(void);

// Funktion: textoutput
void textoutput(char *);

// Funktion: go_on
void go_on(void);

// Funktion: choice
void choice(char *, int, void (*)(), void (*)(), void (*)(), void (*)(), void (*)(), void (*)());

// Funktion: tryyourluck
void tryyourluck(void (*)(), void (*)());

// Funktion: tryyourdexterity
void tryyourdexterity(void (*)(), void (*)());

// Funktion: choose
int choose(char*, int);

// Funktion: escape
void escape(void (*funktion1)());

// Funktion: haveameal
void haveameal(void);

// Funktion: dexteritypotion trinken
void drink_dexteritypotion();

// Funktion: Stärketrank trinken
void drink_strengthpotion();

// Funktion: Glückstrank trinken
void drink_luckpotion();

// Funktion: Objkekt ablegen
void drop_object();

// Funktion: Ja-Nein-Frage
bool yesnoquestion(char *);

// Funktion: raisedexterity
void raisedexterity(int, int);

// Funktion: raisestrength
void raisestrength(int, int);

// Funktion: raiseluck
void raiseluck(int, int);

// Funktion: save
int save(void);

// Funktion: load
int load(void);

// Funktion: quit
void quit(void);

// ---------------------------------
// Implementation für das Spielmodul
// ---------------------------------

void vorwort();
void place1();
void place2();
void place3();
void place4();
void place5();
void place6();
void place7();
void place8();
void place9();
void place10();
void place11();
void place12();
void place13();
void place14();
void place15();
void place16();
void place17();
void place18();
void place19();
void place20();
void place21();
void place22();
void place23();
void place24();
void place25();
void place26();
void place27();
void place28();
void place29();
void place30();
void place31();
void place32();
void place33();
void place34();
void place35();
void place36();
void place37();
void place38();
void place39();
void place40();
void place41();
void place42();
void place43();
void place44();
void place45();
void place46();
void place47();
void place48();
void place49();
void place50();
void place51();
void place52();
void place53();
void place54();
void place55();
void place56();
void place57();
void place58();
void place59();
void place60();
void place61();
void place62();
void place63();
void place64();
void place65();
void place66();
void place67();
void place68();
void place69();
void place70();
void place71();
void place72();
void place73();
void place74();
void place75();
void place76();
void place77();
void place78();
void place79();
void place80();
void place81();
void place82();
void place83();
void place84();
void place85();
void place86();
void place87();
void place88();
void place89();
void place90();
void place91();
void place92();
void place93();
void place94();
void place95();
void place96();
void place97();
void place98();
void place99();
void place100();
void place101();
void place102();
void place103();
void place104();
void place105();
void place106();
void place107();
void place108();
void place109();
void place110();
void place111();
void place112();
void place113();
void place114();
void place115();
void place116();
void place117();
void place118();
void place119();
void place120();
void place121();
void place122();
void place123();
void place124();
void place125();
void place126();
void place127();
void place128();
void place129();
void place130();
void place131();
void place132();
void place133();
void place134();
void place135();
void place136();
void place137();
void place138();
void place139();
void place140();
void place141();
void place142();
void place143();
void place144();
void place145();
void place146();
void place147();
void place148();
void place149();
void place150();
void place151();
void place152();
void place153();
void place154();
void place155();
void place156();
void place157();
void place158();
void place159();
void place160();
void place161();
void place162();
void place163();
void place164();
void place165();
void place166();
void place167();
void place168();
void place169();
void place170();
void place171();
void place172();
void place173();
void place174();
void place175();
void place176();
void place177();
void place178();
void place179();
void place180();
void place181();
void place182();
void place183();
void place184();
void place185();
void place186();
void place187();
void place188();
void place189();
void place190();
void place191();
void place192();
void place193();
void place194();
void place195();
void place196();
void place197();
void place198();
void place199();
void place200();
void place201();
void place202();
void place203();
void place204();
void place205();
void place206();
void place207();
void place208();
void place209();
void place210();
void place211();
void place212();
void place213();
void place214();
void place215();
void place216();
void place217();
void place218();
void place219();
void place220();
void place221();
void place222();
void place223();
void place224();
void place225();
void place226();
void place227();
void place228();
void place229();
void place230();
void place231();
void place232();
void place233();
void place234();
void place235();
void place236();
void place237();
void place238();
void place239();
void place240();
void place241();
void place242();
void place243();
void place244();
void place245();
void place246();
void place247();
void place248();
void place249();
void place250();
void place251();
void place252();
void place253();
void place254();
void place255();
void place256();
void place257();
void place258();
void place259();
void place260();
void place261();
void place262();
void place263();
void place264();
void place265();
void place266();
void place267();
void place268();
void place269();
void place270();
void place271();
void place272();
void place273();
void place274();
void place275();
void place276();
void place277();
void place278();
void place279();
void place280();
void place281();
void place282();
void place283();
void place284();
void place285();
void place286();
void place287();
void place288();
void place289();
void place290();
void place291();
void place292();
void place293();
void place294();
void place295();
void place296();
void place297();
void place298();
void place299();
void place300();
void place301();
void place302();
void place303();
void place304();
void place305();
void place306();
void place307();
void place308();
void place309();
void place310();
void place311();
void place312();
void place313();
void place314();
void place315();
void place316();
void place317();
void place318();
void place319();
void place320();
void place321();
void place322();
void place323();
void place324();
void place325();
void place326();
void place327();
void place328();
void place329();
void place330();
void place331();
void place332();
void place333();
void place334();
void place335();
void place336();
void place337();
void place338();
void place339();
void place340();
void place341();
void place342();
void place343();
void place344();
void place345();
void place346();
void place347();
void place348();
void place349();
void place350();
void place351();
void place352();
void place353();
void place354();
void place355();
void place356();
void place357();
void place358();
void place359();
void place360();
void place361();
void place362();
void place363();
void place364();
void place365();
void place366();
void place367();
void place368();
void place369();
void place370();
void place371();
void place372();
void place373();
void place374();
void place375();
void place376();
void place377();
void place378();
void place379();
void place380();
void place381();
void place382();
void place383();
void place384();
void place385();
void place386();
void place387();
void place388();
void place389();
void place390();
void place391();
void place392();
void place393();
void place394();
void place395();
void place396();
void place397();
void place398();
void place399();
void place400();

// --------------------------------
// Zeigerliste aller ort-Funktionen
// --------------------------------
void (*raumptr[401]) (void) = {vorwort, place1, place2, place3, place4, place5, place6, place7, place8, place9, place10, place11, place12, place13, place14, place15, place16, place17, place18, place19, place20, place21, place22, place23, place24, place25, place26, place27, place28, place29, place30, place31, place32, place33, place34, place35, place36, place37, place38, place39, place40, place41, place42, place43, place44, place45, place46, place47, place48, place49, place50, place51, place52, place53, place54, place55, place56, place57, place58, place59, place60, place61, place62, place63, place64, place65, place66, place67, place68, place69, place70, place71, place72, place73, place74, place75, place76, place77, place78, place79, place80, place81, place82, place83, place84, place85, place86, place87, place88, place89, place90, place91, place92, place93, place94, place95, place96, place97, place98, place99, place100, place101, place102, place103, place104, place105, place106, place107, place108, place109, place110, place111, place112, place113, place114, place115, place116, place117, place118, place119, place120, place121, place122, place123, place124, place125, place126, place127, place128, place129, place130, place131, place132, place133, place134, place135, place136, place137, place138, place139, place140, place141, place142, place143, place144, place145, place146, place147, place148, place149, place150, place151, place152, place153, place154, place155, place156, place157, place158, place159, place160, place161, place162, place163, place164, place165, place166, place167, place168, place169, place170, place171, place172, place173, place174, place175, place176, place177, place178, place179, place180, place181, place182, place183, place184, place185, place186, place187, place188, place189, place190, place191, place192, place193, place194, place195, place196, place197, place198, place199, place200, place201, place202, place203, place204, place205, place206, place207, place208, place209, place210, place211, place212, place213, place214, place215, place216, place217, place218, place219, place220, place221, place222, place223, place224, place225, place226, place227, place228, place229, place230, place231, place232, place233, place234, place235, place236, place237, place238, place239, place240, place241, place242, place243, place244, place245, place246, place247, place248, place249, place250, place251, place252, place253, place254, place255, place256, place257, place258, place259, place260, place261, place262, place263, place264, place265, place266, place267, place268, place269, place270, place271, place272, place273, place274, place275, place276, place277, place278, place279, place280, place281, place282, place283, place284, place285, place286, place287, place288, place289, place290, place291, place292, place293, place294, place295, place296, place297, place298, place299, place300, place301, place302, place303, place304, place305, place306, place307, place308, place309, place310, place311, place312, place313, place314, place315, place316, place317, place318, place319, place320, place321, place322, place323, place324, place325, place326, place327, place328, place329, place330, place331, place332, place333, place334, place335, place336, place337, place338, place339, place340, place341, place342, place343, place344, place345, place346, place347, place348, place349, place350, place351, place352, place353, place354, place355, place356, place357, place358, place359, place360, place361, place362, place363, place364, place365, place366, place367, place368, place369, place370, place371, place372, place373, place374, place375, place376, place377, place378, place379, place380, place381, place382, place383, place384, place385, place386, place387, place388, place389, place390, place391, place392, place393, place394, place395, place396, place397, place398, place399, place400 };

// -------------------------
// Das eigentliche Abenteuer
// -------------------------
void intro(void) {
	int input = 0;

	color_set(0, 0);
	textoutput("Hinweis\nIn diesem Roman wird niemand anderes als du selbst die Person sein, die das Abenteuer durchlebt. Von daher würde ich sagen, würdest du dir selber beim Erleben der Spielatmosspähre helfen, wenn du die Spielfigur nach dir benennst, oder ihr einen Namen gibst, der dir gefällt oder den du gerne tragen würdest.\nViel Spaß beim Lesen und Spielen!\nSascha Karl");
	color_set(1, 0);
	printw("By what name should you be known in the story? ");
	getnstr(player.name, 30);
	color_set(0, 0);
	player.dexterity_start = dice(6) + 6;
	player.dexterity = player.dexterity_start;
	player.strength_start = dice(6) + dice(6) + 12;
	player.strength = player.strength_start;
	player.luck_start = dice(6) + 6;
	player.luck = player.luck_start;
	textoutput("Zu Beginn dieses Abenteuer wirst du nur ein absolutes Minimum an Objekten bei dir führen, als da wären ein Rucksack, ein Multifunktionstaschenmesser, eine Pumptaschenlampe und etwas Proviant. Außerdem ein Engergydrink, den du dir selber auswählen kannst. Welchen der Energydrinks wählst du?\n(1) den dexteritypotion - er stellt die anfängliche Gewandheitspunktzahl wieder her\n(2) den Stärketrank - er stellt die anfängliche Stärkepunktzahl wieder her\n(3) den Glückstrank - er stellt die anfängliche Glückspunktzahl wieder her und verbesser sie zusätzlich um einen Punkt");
	while((input < 1) || (input > 3)) {
		input = choose("Which energy drink would you choose for yourself? ", 3);
		switch(input) {
			case 1: object[dexteritypotion] += 2;
					break;
			case 2: object[strengthpotion] += 2;
					break;
			case 3: object[luckpotion] += 2;
					break;
			default: color_set(4, 0);
					 printw("Fehler! In der Energydrink-choice,  in Funktion intro().");
					 color_set(0, 0);
					 break;
		}
	}
}

void vorwort(void) {
	textoutput("Vorwort\n\nViele Menschen würden der Wiedervereinigung und der Politik die Schuld geben, das Menschen ins Abseits geraten. Die meisten wissen nicht, was das wirklich bedeutet, du jedoch schon. Seit Jahren bereits kämpfst du um dein Überleben, mit nicht mehr als deiner Kleidung am Körper, einem Ruckscack, einem Zelt, einer Pumptaschenlampe, einem Multifunktionstaschenmesser billigster Machart und einigen wenigen weiteren Kleinigkeiten. Momentan ist das Wetter noch Mild - und so hast du die Nacht in den Überresten einer alten Turmruine verbracht, die unbeachtet im Park des Museums am Münster steht.\nDoch diese Nacht war anders, als jede andere Nacht, die du im Freien verbracht hast. Sie war unheimlich. Du kannst dich erinnern, wie du aus dem Zelt klettertest - aber es war noch dunkel, und pechschwarz. Der ganze Himmel, alles war nur Schwarz, kein einziger Stern war zu sehen, nicht die Silhouette des Mondes, keine Wolken - rein gar nichts! Und dann war da dieses röhrende, metallische Geräusch. Und noch bevor du weißt, wie dir geschieht, wird es langsam hell - und du stehst immer noch da, in der gleichen Position. Bewegungslos. Die Frage ist nur, wie lange schon hast du bewegungslos herumgestanden? Und wieso überhaupt konntest du dich nicht bewegen?\nDa es noch immer recht früh ist, gehst du hinunter zu dem kunstvollen Brunnen des Museums. Der Park gehört in diesen Stunden dir allein. Du nimmst ein kurzes Bad und fühlst dich direkt viel wohler, so sauber, wenn es auch ein klein wenig eiskalt war. Mit einem alten Handtuch reibst du dich notdürftig trocken und ziehst dir saubere Unterwäsche an, bevor der Rest deiner alltäglichen Kleidung folgt. Den Rest deiner Kleidung verstaust du in einem Stoffbeutel und gehst zurück zu deinem Zelt. Mit wenigen Handgriffen baust du es auseinander, dann faltest du es und verstaust es vorsichtig.\nMit deinem ganzen Habseligkeiten bepackt gehst du die Treppen des Münster hinunter. Als Kind hast du dafür immer den schnellen Weg gewählt, mit den glatten Sandahlensohlen seid deine Freunde und ihre einfach immer stehend das Geländer hinuntergerutscht. Kinder kenen keinerlei Angst vor Gefahren. Du überquerst den Platz hinüber in Richtung des ehemaligen Dortin-Hotels und schlägst den Weg, die Hügelstraße hinauf ein. Nahe dem alten Wasserturm am Odenkirchweg versteckst du den Rucksack mit deinem Zelt, der Isoliermatte und all den anderen Dingen, die du nur des Nachts brauchst - und wanderst nur mit deinem \"leichten Gepäck\" die Vitusstraße hinunter. Du wanderst die Lüperzender Straße entlang und gehst schließlich die Brücke am ehemaligen Zentralbad hinauf - um noch kurz den kleine Park zu durchqueren, bevor du in Kürze die Hindenburgstraße nahe des Stadttheaters, oder einem seiner Nachfahren erreichst.");
}

// -----------
// --- 001 ---
// -----------

void place1(void) {
	room = 1;
	textoutput("Die Innenstadt macht um diese Zeit noch einen gänzlich unbelebten Eindruck - und kein einiges Licht erhellt die Straße. Keine Straßenlaterne, keine Glühbirne hinter einem Fenster, nicht einmal die Werbetafeln leuchten auf. Wenn du so drüber nachdenkst, auf deinem ganzen Weg hierhin hast du bisher keinerlei eingeschaltete Lichtquellen gesehen.\nNun stehst du an der Kreuzung Stepgesstraße Ecke Hindenburgstraße. Ein Stück die große Einkaufsstraße hinauf ist das Bäckereigeschäft, an dem dir jeden Morgen ein guter Geist eine braune Papiertüte mit dampfend warmen Gebäck bereitstellt. Du schaust in die Richtung, doch diesmal steht auf dem Stromkasten neben der Türe nicht der verlockende Beutel, sondern eine Gestalt steht in der Türe und winkt dir zu.");
	choice("Wirst du auf die winkende Gestalt zugehen (1) oder ziehst du es vor, dich lieber scheu und mit knurrendem Magen aus dem Staub zu machen?", 2, place2, place3, NULL, NULL, NULL, NULL);
}

void place2(void) {
	room = 2;
	textoutput("Vorsichtig und etwas nervös näherst du dich der offenstehenden Ladentüre, als ein Regelrechter Platzregen losbricht, dann hört man es auch donnern. now läufst du auf die Türe zu. Eine Frau im weißen Kitel geht hinein und hält sie dir offen. Du gleitest hinein und bleibst neben einem Stehtisch stehen, während sie hinter dir die Türe zumacht und abschließt.\n\"Ist das nicht ein unheimlicher Morgen? Die ganze Stadt ist ohne Strom. Momentan habe ich ein Feuer im alten Backofen gemacht - gleich wird es hier ein paar frische Brötchen geben.\" sagt sie und geht an der Theke vorbei auf eine Tür die nach hinten führt zu. \"Na kommen sie!\" lädt sie dich ein, ihr zu folgen. Du gehst unsicher hinterher. Im Hinterzimmer steht ein Tisch, auf dem eine Thermoskanne und zwei Tassen bereitstehen. \"Ich wollte sie schon längst einmal angesprochen haben. Daß das an einem so schlechten Tag passiert, tut mir leid!\" Sie kommt mit einem Tablet voller dampfender Brötchen aus einem Nebenraum, während du unschlüssig neben dem Tisch stehst.\n\"Bitte setzen sie sich doch! Ich dachte, wir könnten zusammen frühstücken!\" sagt sie und stellt das Tablett auf dem Tisch ab. Dann setzt sie sich hin. \"Ich bin Elke.\" stellt sie sich vor.\nAls du ihr gerade mit deinem Namen antworten willst, geschieht das unfassbare: die Sirenen fangen an zu heulen, mitten in dieser Kackophonie des Gewitters - und das um Fünf Uhr, Morgens in der Frühe.\n\"Was geht da vor sich?\" fragt Elke, während du dir das Sirenensignal genauer anhörst. \"Haben sie ein Radio hier, oder einen Fernsehe?\" fragst du Elke, aber die zuckt nur mit den Schultern.\n\"Kein Batteriebetriebenes, der Strom ist ausgefallen, da tut's der Fernseher nicht.\" Sie deutete mit der Hand neben deinem Ohr entlang hinter dich. Du drehst dich auf der Sitzfläche des Stuhls um und erblickst das Gerät. Die Sirenen heulen now bereits seit mehr als einer Minute.\n\"Das ist keine Übung, das ist eine echte Warnung, aber vor was?\" sagst du laut zu dir. Du sprichst sehr oft mit dir selbst, vermutlich, um nicht vor Einsamkeit den Verstand zu verlieren. Wieder erklingt das Donnern und Grollen.");
	choice("Willst du erst einmal in Ruhe Frühstücken (1) oder Elke fragen, ob sie dich rauslassen kann, damit du nachsiehst, was eigentlich los ist (2) oder sie fragen, ob sie über einen weiteren Empfänger verfügt, der nicht ans Stromnetz angeschlossen ist (3)?", 3, place4, place5, place6, NULL, NULL, NULL);
}

void place3(void) {
// Kein Kontakt
	room = 3;
	textoutput("Du bist ein äußerst mißtrauischer Mensch und vertraust der unbekannten, winkenden Gestalt nicht. Zu deinem Glück fängt es an zu regnen. Es ist ein echter Platzregen. Du rennst ein Stück weit die Hindenburgstraße hinab und biegst in die Lichtpassage ein, wo du dich während des Regens unterstellts. Kaum bist du eine Minute drin, als plötzlich überall die Sirenen anfangen zu heulen. Draußen hörst du ein lautes Donnergrollen. Auch ist es viel dunkler geworden, seit die dichten grauen Wolken sich mit ihrem Regen auf Gladbach ergehen. Plötzlich pfeift ein Schuß an dir vorbei und schlägt in die Fensterscheibe ein Stück vor dir ein. Du hechtest in den nächsten Eingang und siehst zurück. Du bist nicht alleine. Von oben herab kommt eine Gestalt gelaufen. Aus deinem Rucksack holst du dein Taschenmesser heraus und klappst es auf. Es ist fast so, als wärst du unbewaffnet, aber besser als nichts. Dein enemy kommt go_on auf dich zugeeilt.");
	place13(); // Zufallsgegner
	textoutput("Die gun deines Gegners ist leergeschossen. Du durchsuchst ihn hastig, findest, aber keinerlei Munition bei ihm.");
	if(yesnoquestion("Möchtest du die gun einstecken (j/n)?"))
		object[gun] += 1;
	textoutput("Es war zwar Notwehr, dennoch hockst du now hier neben einer Leiche. Dein Gefühl sagt dir, daß es wohl das beste wäre, dich so schnell wie möglich aus dem Staub zu machen, Unwetter hin oder her.");
	choice("Möchtest du den Lichthof nach Norden verlassen in Richtung Kaisterstraße (1), oder nach Süden in Richtung Hindernburgstraße (2)?", 2, place50, place51, NULL, NULL, NULL, NULL);
}

void place4(void) {
// Frühstücken
	room = 4;
	textoutput("Du setzt dich zu Elke an den Tisch. Das Wetter, den Donner und den fehlenden Strom ignorieren, lernt ihr euch langsam besser kennen, während die Anzahl der Backwaren auf dem Tablett deutlich schrumpfen und der heiße Kaffee aus der Thermoskanne sich wohlig in deinem Inneren verteilt. Der Ort, die Zeit, die Situation, alles könnte man als idyllisch bezeichnen, wenn, ja wenn nicht auf einmal dieses seltsame Geräusch eine absterbenden Sirene gewsen wäre. Es war kein Abschwellen, wie man es kenn, sondern klang eher, wie ein entenartiger Aufschrei.\nUnd dann nimmst du plötzlich war, das mit dem Verstummen dieser Sirene die Masse an Sirenen, die noch klingen, weniger hoch ist, als zuvor. Aber was viel wichtiger ist, einige gehen immer noch. Langsam wirst du dir der Situation bewußt, die da draußen herrscht - und du beschließt, nachsehen zu gehen, was da los ist.");
	place5();
}

void place5(void) {
// Raus auf die Straße
	room = 5;
	textoutput("Du schlägst vor, daß du dich draußen umsiehst und zurückkommst, sobald du weißt, was los ist. Elke begleitet dich zur Ladentüre und läßt dich raus. Der Regen prasselt von oben herab und immer wieder donnert es. Du winkst ihr kurz zu und rennst so gut es geht an den Hauswänden entlang die Hindenburgstraße hinauf, trotzdem bist du nach weniger als einer Minute bis auf die Unterhose durchnäßt. Als du am ehemaligen Heinemann vorbeikommst und durch die kurze Passage läuftst, bemerkst du an der Straßenecke zum Sankt Vith hinunter einen brennenden Polizeiwagen. Ein mulmiges Gefühl geht dir durch den Magen. Eigentlich wolltest du ja in das Haus, das früher deinem Großvater gehört hat - und von dem aus man eine Übersicht über die ganze Stadt hat. Trotzdem ergreift dich gerade die Angst.");
	choice("Vielleicht wäre es ja besser, die Straße wieder hinunter zulaufen - und sich im Geschäft bei Elke zu verstecken (1)? Du könntest auch auf den Polizeiwagen zulaufen, vielleicht kannst du jemandem helfen, der im Wagen eingeklemmt ist (2)? Oder du folgst deinem ursprünglichen Plan, das Haus deines Großvaters zu erreichen (3)?", 3, place7, place8, place9, NULL, NULL, NULL);
}

void place6(void) {
// Nach einem Empfänger fragen
	room = 6;
	textoutput("Elke schüttelt den Kopf, und verneint deine Frage. \"Tut mir leid, so etwas ist nicht hier im Geschäft. Das einzige Radio, das mir einfallen würde, wäre mein Autoradio.\" entgegnet sie, nimmt sich ein warmes Brötchen vom Tablett und kaut daran herum.");
	choice("Willst du sie wirklich überreden, mit dir zu ihrem Auto zu gehen (1), oder würdest du now nicht viel lieber frühstücken (2)? Wenn du unbedingt draußen nachsehen willst, was los ist, könntest du Elke auch bitten, dich rauszulassen (3).", 3, place17, place4, place5, NULL, NULL, NULL);
}

void place7(void) {
// Zurück zum Geschäft
	room = 7;
	textoutput("Vollkommen durchnäßt kommst du wieder am Geschäft an. Drinnen ist alles dunkel. Du klopfst mehrfach, aber nichts rührt sich.");
	choice("Willst du es go_on mit klopfen und rufen probieren (1), oder willst du versuchen, ob du die Türe öffnen kannst (2), oder willst du dir einen anderen Weg suchen (3)?", 3, place14, place15, place16, NULL, NULL, NULL);
}

void place8(void) {
// Zum Polizeiwagen
	room = 8;
	if(object[riffle] > 0) {
		textoutput("Du läufst erneut über den Platz um dir den Polizeiwagen noch einmal anzusehen. Das Feuer ist erloschen. Du überlegst dir gerade, ob du eine noch gründlichere Durchsuchung des Wagens vornehmen sollst, als du ein Geräusch aus der Richtung der Kirche hören kannst. Ein Geschoss schlägt in die Karosserie des Wagens ein.");
		choice("Willst du dich dem fight stellen (1) oder versuchen zu fliehen (2)?", 2, place10, place11, NULL, NULL, NULL, NULL);
	}
	else {
		textoutput("Der Polizeiwagen raucht eigentlich nur noch. Fahrer- und Beifahrertüre stehen weit offen. Vorsichtig umrundest du den Wagen. Kein Mensch ist darin, aber auch kein Anzeichen der beiden Polizisten, die sich darin hätte befinden müssen. Im offenen Seitenfach der Beifahrertüre ist ein riffle, die Verriegelung der Waffe ist offen. Du könntest sie an dich nehmen. Du durchsuchst den Kofferraum. In diesem befindet sich ein Munitionspäckchen mit 100 Patronen, ein Verbandskasten und ein Warndreieck.");
		if(yesnoquestion("Willst du etwas von den Sachen mitnehmen (j/n)?")) {
			if(yesnoquestion("Willst du das riffle mitnehmen (j/n)?"))
				object[riffle] += 1;
			if(yesnoquestion("Willst du die Patronen mitnehmen (j/n)?"))
				object[ammunition] += 100;
			if(yesnoquestion("Willst du den Verbandskasten mitnehmen (j/n)?"))
				object[verbandskasten] += 1;
			if(yesnoquestion("Willst du das Warndreieck mitnehmen (j/n)?"))
				object[warndreieck] += 1;
		}
		choice("Willst du now go_on zum Haus deines Großvaters (1) oder zurück zum Geschäft (2)?", 2, place9, place7, NULL, NULL, NULL, NULL);
	}
}

void place9(void) {
// Zum Haus deines Großvaters
	room = 9;
	if(tripodgesehen == true) {
		textoutput("Du läufst auf das Haus deines Großvaters zu und in den Eingang zwischen Gaststätte und Kleidungsgeschäft. Für einen Moment mußt du vor der alten Holztüre mit der Glasscheibe halten. Mit Hilfe deines Taschenmessers daudert es nur ein paar Sekunden, dann ist die Türe öffnet und du schlüpfst hinein.");
		place21();
	}
	textoutput("Du läufst an der Häuserwand des Alten Marktes entlang und gelangst schließlich zum Haus deines Großvaters. Du machst dich mit Hilfe deines Taschenmessers kurz am Schloß der alten Holztüre zu schaffen, sie geht fast sofort aus. Den Trick hast du schon als Kind gemacht, wenn du mal wieder deinen Schlüssel zu Hause vergessen hattest - und er klappt immer noch wunderbar. Du hastest die Türe hinauf. Immer noch donnert es draußen, so laut, wie du es schon lange bei keinem Gewitter mehr gehört hast. Auf jeder Etage drückst du den Lichtschalter, aber keiner schaltet das Licht an. Auch hier ist vollkommener Stromausfall. Kurz bevor du die oberste Etage erreichst, hören die Sirenen auf zu heulen, was für dich nur zum Vorteil sein kann, steht doch noch eine alte motorbetriebene E57. Du obersten Treppen sind am kürzesten und am verwinkelsten. Links die Waschküche läßt du liegen, da kannst du nichs sehen. Du stürmst nach rechts, den kurzen Gang entgang und reißt die hintereste Türe auf der rechten Seite auf. Du stürmst auf das Dachfenster an der Südseite zu. Überall siehst du dunkelgraue Wolkenberge sich auftürmen. Statt bis nach Düsseldorf kannst du nicht einmal bis an die Westgrenze Mönchengladbachs sehen. Du wendest den Blick ab und läufst zu einem der Fenster an der Nordseite. Hier bietet sich die ein gleiches Bild. Die Wolken sind so dicht, daß du nicht einmal den Gladbacher Wasserturm in dieser Brühe sehen kannst. Bleiben noch die Fenster an der Südseite. Bereits als du dich ihnen näherst, erkennst du, daß du hier ein weiteres Sichtfeld haben wirst. Du reißt das Fenster auf um besser sehen zu können. Von oben peitschen dicke Regentropen herab, aber das aufgeklappte Fenster schützt dich weitestgehend. Die Wolkenwand ist hier einige Kilometer entfernt. Da plötzlich wird es hell in der Wolkenwand. Wie gebannt starrst du dahin. War das ein Blitz? Da wieder. Wieder ein Blitz. Wieder in der Wolkenwand. Das ist now aber sehr ungewöhnlich. Minutenlang starrst du auf die Wolkenwand - und auch an zwei oder drei anderen Stellen erblickst du immer wieder kurze Blitze - mitten in der Wolkenwand, aber fast auf Bodenhöhe. Ein mulmiges Gefühl breitet sich in deinem Magen aus. Gerade, als du das Fenster schließen willst, kommt etwas aus der Wolke. Etwas riesiges. Ein object, wie ein Turm, aber es bewegt sich, wie ein Stelzenläufer. Und ein Lichtbogen, wie ein Blitz spannt sich von ihm aus, dann brennt etwas zu seinen - Beinen? - während es sich go_on in Richtung der Gladbacher Innenstadt voranschiebt.\n Deine Nackenhärchen haben sich aufgerichtet. Du weißt zwar nicht genau, was das ist, aber es bringt Zerstörung, soviel ist sicher. Hastig schließt du das Fenster. Du rennst aus dem Dachstuhl heraus zurück in den Flur und eilst die Treppen hinab, bis du unten an den der Haustüre ankommst. Was wirst du tun?");
	tripodgesehen = true;
	choice("Du läufst hinaus und zu dem Polizeiwagen (1), du läufst die Kellertreppe hinab und suchst dort Schutz vor dem was kommt (2) oder du läufst zurück zu Elke, der Frau, die dich täglich mit Backwaren versorgt hat und erzählst ihr, was du gesehen hast (3)?", 3, place8, place20, place7, NULL, NULL, NULL);
}

void place10(void) {
// fight
	room = 10;
	place13();
	textoutput("Du bist nicht stolz darauf, einen anderen Menschen getötet zu haben, aber du warst ja nicht der attacker. Trotzdem fühlst du dich schäbig. Etwas in dir hat sich verändert, das kannst du spüren. Noch immer prasselt der Regen auf dich, so als wäre nichts gewesen. Und hinter den Wolkenbergen, da bist du dir sicher, scheint immer noch die Sonne.");
	choice("Willst du now go_on zum Haus deines Großvaters (1) oder zurück zum Geschäft (2)?", 2, place9, place7, NULL, NULL, NULL, NULL);
}

void place11(void) {
// fliehen
	room = 11;
	tryyourluck(place12, place13);
	place10();
}

void place12(void) {
// entkommen
	room = 12;
	textoutput("Geduckt rennst du an der Häuserwand entlang und dann rechts in die Straße, auf das Kabuff zu, dort rennst du links um die Ecke in Richtung Kapuzinerplatz, läufst dann aber stattdessen um die Kirche herum, ein kurzes Stück über den Parkplatz - und dann die Stepgesstraße hinunter, bis du schließlich wieder die Hindenburgstraße erreichst. Etwas atemlos stellst du dich an die Wand. Mehr als nur einmal bist du beim Laufen geschlittert. Das Wasser läuft in Strömen in die Kanalisation. Du vergewisserst dich, bleibst mehrere Minuten in deiner versteckten Position, bis du dir sicher bist, daß der attacker dir nicht gefolgt ist. Erst now begibst du dich zurück zur Bäckerei.");
	place7();
}

void place13(void) {
	// Begegnung mit einem Zufallsgegner der "Zivilseite"
	int zufallsgegner = dice(6);
	bool kampfausgang;
	character_t gegner1 = { "verängstigte Frau", 2, 2, 3, 3 };
	character_t gegner2 = { "Plünderer", 6, 6, 3, 3 };
	character_t gegner3 = { "aggressiver Jugendlicher", 6, 6, 4, 4 };
	character_t gegner4 = { "Polizist", 5, 5, 4, 4 };
	character_t gegner5 = { "Gutbürger", 6, 6, 5, 5 };
	character_t gegner6 = { "Heckenschütze", 8, 8, 4, 4 };
	character_t gegner7 = { "wahnsinniger Polizist", 6, 6, 4, 4 };
	switch(zufallsgegner) {
		case 1: kampfausgang = fight(&player, &gegner1, 1, false, NULL);
				break;
		case 2: kampfausgang = fight(&player, &gegner2, 1, false, NULL);
				break;
		case 3: kampfausgang = fight(&player, &gegner3, 1, false, NULL);
				break;
		case 4: kampfausgang = fight(&player, &gegner4, 1, false, NULL);
				break;
		case 5: kampfausgang = fight(&player, &gegner5, 1, false, NULL);
				break;
		case 6: kampfausgang = fight(&player, &gegner6, 1, false, NULL);
				break;
		default: kampfausgang = fight(&player, &gegner7, 1, false, NULL);
				break;
	}
	if(!kampfausgang) {
		textoutput("Das war nicht dein bester fight. Um ehrlich zu sein, das war dein schlechtester fight - und auch dein letzter fight. Dein allerletzter fight, den du nicht überlebt hast. Mit dir ist es zu ENDE gegangen.");
		exit(EXIT_SUCCESS);
	}
	killedpeople += 1;
	if(dice(6) >= 4) {
		if(object[gun] <= 1)
			if(yesnoquestion("Willst du die gun deines Gegners an dich nehmen (j/n)?"))
				object[gun] += 1;
	}
	else {
		if(object[riffle] <= 0)
			if(yesnoquestion("Willst du das riffle deines Gegners an dich nehmen (j/n)?"))
				object[riffle] += 1;
	}
	object[ammunition] += dice(8);
}

void place14(void) {
// Klopfen und rufen
	room = 14;
	if(dice(6) >= 4) {
		textoutput("Dein Klopfen hat dich unvorsichtig gemacht. Du hast einen unerwarteten Gast angelockt.");
		place13();
	}
	else textoutput("Bisher scheinen deine Bemühungen nicht zu fruchten. Drinnen reagiert Nichts und niemand auf dein Klopfen.");
	choice("Willst du es go_on mit klopfen und rufen probieren (1), oder willst du versuchen, ob du die Türe öffnen kannst (2), oder willst du dir einen anderen Weg suchen (3)?", 3, place14, place15, place16, NULL, NULL, NULL);
}

void place15(void) {
// Versuchen, die Türe aufzubekommen
	room = 15;
	tryyourluck(place18, place19);
}

void place16(void) {
// Versuchen, einen anderen Weg zu finden
	room = 16;
}

void place17(void) {
// Auf dem Weg zu Elkes Auto
	room = 17;
	textoutput("Du schulterst dir deinen Rucksack, während Elke ein paar Papiertüten mit Backwaren füllt. Danach siehst du ihr zu, wie sie ein paar Flaschen aus dem Kühlschrank holt und in eine weitere Tüte stopft. Als sie alles gepackt hat, verschwindet sie kurz im Nebenzimmer und kommt, now einen Mantel tragend, eine Damenhandtusche um die Schulter hängend zurück. Ihr nennt die ganzen Taschen und verlasst das Geschäft. Elke schließt den load hinter sich ab, macht sogar die Bewegung zum Scharfschalten der Alarmanlage, dann sagt sie: \"Komm!\" und geht dir voraus, die Stepgesstraße hinunter.\nMißtrauisch wirfst du einen Blick zum Jugendheim hinüber. Oft genug, haben dir da schon junge Kerle aufgelauert. Diesmal aber bleibt alles ruhig. Elke führt dich ein kurzes Stück go_on und hält vor einer Parkhaustüre. Sie holt einen Schlüssel heraus und schließt auf. Gemeinsam geht ihr in das Innere des Parkhauses. Das Treppenhaus stinkt nicht so muffig und nach Urin, wie es oft der Fall ist, wenn Betrunkene sich eine Stelle für ihre Notdurft suchen. Ihr steigt hinab bis zum dritten Untergeschoß.\nSchließlich erreicht ihr Elkes Auto, einen schon etwas älteren Kombi. Sie entriegelt das Auto per Knopf. Ihr verladet eure Taschen und deinen Rucksack im Kofferraum und setzt euch in das Auto, wobei du auf dem Beifahrersitz Platz nimmst. Das kleine Licht der Mittelkonsole ist im Moment die einzige aktive Lichtquelle im ganzen Parkhaus.\nElke fummelt am Autoradio herum, aber das Digitalradio findet keinen Sender den es darstellen kann. Schließlich gibt sie die Suche auf. \"Hier drinnen haben wir keinen Empfang, ich müßte schon aus dem Parkhaus herausfahren, damit wir etwas hören könnten.\"");
	choice("Stimmst du Elke zu - und ihr fahrt mit dem Auto aus dem Parkhaus heraus in den Regen (1) oder hast du ein mulmiges Gefühl und glaubst nicht, daß das eine gute Idee ist (2)?", 2, place100, place101, NULL, NULL, NULL, NULL);
}

void place18(void) {
// Du bekommst die Türe auf
	room = 18;
}

void place19(void) {
// Die Türe geht nicht auf
	room = 19;
}

void place20(void) {
// Im Keller des Großvaters
	room = 20;
	textoutput("Du öffnest die eiserne Treppe und gehst die Kellertreppe hinunter. Als du am Fuße der Treppe ankommst, siehst du vor dir die eiserne Doppeltüre, die in den Heizungskeller führt. Nach rechts führt ein weiterer Weg zum Lagerraum, wo deine Großmutter ihre Kartoffeln lagerte. Neben der Treppe führt ein Weg nach hinten, wo die Mieter des Hauses ihre Kellerabteile haben.");
	choice("Wenn du zurück nach oben gehen willst (1). Möchtest du in den Heizungskeller (2). Willst du in den Gang, der zum Kartoffellager deiner Großmutter führt (3). Würdest du gerne in den rückwärtigen Bereich gehen (4).", 4, place21, place22, place23, place24, NULL, NULL);
}

void place21(void) {
// Treppenflur
	room = 21;
	textoutput("Du stehst in einem großen Treppenflur. An der Seite befindet sich der Hängeschrank mit den ganzen Ablesegeräten für Elektrizität und Wasser. Rechts an der Wand für eine Steintreppe hinauf in das erste Stockwerk. Geradeaus ist eine Holztüre, durch die man in den Kellerbereich der Gaststätte kommt, allerdings ist sie dauerhaft abgeschlossen. Rechts neben der Holztüre, unterhalb der Steintreppe, befindet sich eine Eisentüre, hinter der sich der Abstieg in den Keller befindet.");
	choice("Willst du die Treppe hinaufsteigen (1), in den Keller hinuntergehen (2), oder das Haus verlassen und zurück auf den Alten Markt (3)?", 3, place25, place20, place26, NULL, NULL, NULL);
}

void place22(void) {
// Heizungskeller
	room = 22;
	if(agartha == true)
		textoutput("Ein großteil der Kohlen liegt auf dem Boden. An der rußgeschwärtzten Wand kannst du das Wort \"Agartha\" lesen. Der Heizungskessel arbeitet derzeit nicht. Sonst befindet sich ncihts im Heizungskeller.");
	else {
		textoutput(" Als du die Türe öffnest, ist es recht stickig im room. Der Heizkessel ist aus, obwohl in einem großen Verschlag an der Seite sich die Kohlen nur so stapeln. Als du genauer hinguckst, hast du das Gefühl, den geritzen Buchstaben \"A\" an der Wand zu sehen.");
		if(yesnoquestion("Willst du soviele der Kohlen zur Seite räumen, um zu sehen, ob das \"A\" vielleicht Teil eines Wortes ist? Eine letzte Nachricht deines Großvaters an dich?")) {
			textoutput("Deine Kleidung verdreckt vollständig, während du auf den Kohleberg krabbelst - und Brikett umd Brikett wegräumst. Noch dazu machst du eine Menge Krach.");
			if(dice(6) >= 4)
				place13();
			textoutput("Endlich hast du das Gekritzel an der Wand freigelegt. Was du liest, ist ein Wort: \"Agartha\"");
			agartha = true;
		}
	}
	choice("Willst du in in den Gang zum Kartoffelkeller (1), oder willst du zu den rückwärtigen Kellen (2) oder möchtest du zurück nach oben in den Treppenflur (3)?", 3, place23, place24, place21, NULL, NULL, NULL);
}

void place23(void) {
// Gang zum Kartoffelkeller
	room = 23;
	textoutput("Du gehst durch den nach Moder riechenden Gang. Als du an das ehemalige Kartoffellager deiner Großmutter kommst, mußt du feststellen, das es einfach nur leer und verdreckt ist. Seit ihrem Tod hat es wohl niemand mehr betreten. Die Kartoffeln müssen schon vor Jahren verschimmelt sein. Du wendest den Blick zur Seite. Die Klappe, durch die früher immer die Bierfässer hinabgelassen wurden, befindet sich immer noch am Ende des Ganges. now würde sie allerdings in den Wintergarten der Gaststätte führen. Da du hier nichts go_on finden kannst, beschließt du zurückzugehen.");
	place20();
}

void place24(void) {
// Rückwärtige Keller
	room = 24;
	textoutput("Du gehst an der Kellertreppe vorbei. Hinter ihr macht der Gang einen Knick nach rechts, und nach einem kurzen Durchang stehst du vor einem breiten Gang. Überall sind durch hölzerne Boxen getrennt die Kellerabteile der Mieter dieses Hauses. Der Verschlag, hinter dem deine Sachen gesichert waren, ist leer, bis auf die Erinnerungen daran in deinem Kopf. Du drehst dich um und gehst zurück.");
	place20();
}

void place25(void) {
// Die Treppe hinaufsteigen
	room = 25;
	if(schluesselgefunden) {
		textoutput("Die steigst die Treppen hinauf bis zum Dachgeschoss. Hinter der vorletzten Türe auf der rechten Seite, befindet sich das Spielzimmer, daß dein Großvater dir schenkte. Hier hatte er auf einer Spanplatte eine Eisenbahn mit Bergen und Tunneln für dich aufgebaut, auch wenn er es meistens war, der die Züge fahren ließ. Du steckst den Schlüssel, den du im Keller gefunden hast, ins Schloss - und schließt die Türe auf. Du öffnest sie - und vor dir schwingt sie nach innen. Die Eisenbahn ist immer noch fest montiert auf der Spanplatte, die auf dem rötlichen Boden liegt. Du trittst ein - und schließt die TÜre hinter dir, schließt sie sogar ab, um ungestört zu sein. Auf einem Holzstuhl findest du ein Buch. Als du es dir nimmst und umdrehst, ist es der fehlende Band aus der Bibliothek. Groß prangt der Name Agartha darauf. Du setzt dich auf den Stuhl und blätterst es durch. Immer wieder stößt du auf Passagen in dem Buch, die dein Großvater mit Bleistift unterstrichen hat. In den Passagen ist die Rede von einer Stadt, die sich in den Tiefen der Erde befinden soll. Es finden sich Referenzen darauf, das es Eingänge zu dieser Stadt geben soll - und das wenigstens einer dieser Eingänge in Tibet ist. Plötzlich flattert ein Blatt aus dem Buch. Es muß zwischen den Seiten gelegen haben. Du bückst dich und hebst es auf. Mit der krakeligen Schrift deines Großvaters steht darauf notiert: \"Münster   Sarg   Innenhof   Keller   Rheydt\"\nDu steckst den Zettel und das Buch in deinen Rucksack.");
		buchgefunden = true;
	}
}

void place26(void) {
// Der Alte Markt
	room = 26;
}


void place27(void) {
}

void place28(void) {
}

void place29(void) {
}

void place30(void) {
	room = 30;
}

void place31(void) {
}

void place32(void) {
}

void place33(void) {
}

void place34(void) {
}

void place35(void) {
	room = 35;
}

void place36(void) {
	room = 36;
}

void place37(void) {
	room = 37;
}

void place38(void) {
}
void place39(void) {
}

void place40(void) {
	room = 40;
}

void place41(void) {
}

void place42(void) {
	room = 42;
}

void place43(void) {
	room = 43;
}

void place44(void) {
}

void place45(void) {
}

void place46(void) {
	room = 46;
}

void place47(void) {
}

void place48(void) {
	room = 48;
}

void place49(void) {
}

void place50(void) {
// Alleine go_on, Richtung Kaiserstraße
	room = 50;
}

void place51(void) {
// Alleine go_on, Richtung Hindenburgstraße
	room = 51;
}

void place52(void) {
	room = 52;
}

void place53(void) {
}

void place54(void) {
	room = 54;
}

void place55(void) {
}

void place56(void) {
}

void place57(void) {
	room = 57;
}

void place58(void) {
	room = 58;
}

void place59(void) {
}

void place60(void) {
}

void place61(void) {
}

void place62(void) {
	room = 62;
}

void place63(void) {
	room = 63;
}

void place64(void) {
}

void place65(void) {
}

void place66(void) {
	room = 66;
}

void place67(void) {
	room = 67;
}

void place68(void) {
}

void place69(void) {
}

void place70(void) {
}

void place71(void) {
}

void place72(void) {
}

void place73(void) {
}

void place74(void) {
}

void place75(void) {
}

void place76(void) {
}

void place77(void) {
	room = 77;
}

void place78(void) {
	room = 78;
}

void place79(void) {
	room = 79;
}

void place80(void) {
	room = 80;
}

void place81(void) {
}

void place82(void) {
}

void place83(void) {
}

void place84(void) {
}

void place85(void) {
	room = 85;
}

void place86(void) {
}

void place87(void) {
}

void place88(void) {
	room = 88;
}

void place89(void) {
}

void place90(void) {
}

void place91(void) {
}

void place92(void) {
}

void place93(void) {
}

void place94(void) {
	room = 94;
}

void place95(void) {
}

void place96(void) {
}

void place97(void) {
	room = 97;
}

void place98(void) {
}

void place99(void) {
}

// -----------
// --- 100 ---
// -----------

void place100(void) {
// Du stimmst Elke zu und ihr verlaßt das Parkhaus
	room = 100;
	textoutput("Ihr verlaßt das Auto. Elke fährt den Wagen die Stepgesstraße hinunter und hält vor der Ampel. Sobald Grün kommt, überquert sie die Kreuzung und fährt den Berliner Platz entlang. Du schaust hinüber zum Kaiserbad, wo du als kleines Kind schwimmen gelernt hast.\n\"Schaltest du mal das Radio an?\" bittet sie dich und du wendest dich der Mittelkonsole zu. Kurz darauf ist das Digitalradio an. \"Mit Radio 90,1 wird das nichts. Der Sender hat wohl keinen Saft.\" kommentierst du den vorausgewälten Sender und probierst einen Senderplatz nach dem anderen. Schließlich empfängt das Radio ein Signal auf der Deutschen Welle.\n\"... wird im Westen der Republik von verheerenden Unwettern berichtet, die auch im Venloer room bereits für erhebliche Zerstörung gesorgt haben sollen. Fast die ganze Niederlande und der Niederrhein verfügen über keinerlei Strom mehr. Auch die offiziellen Behördennetzwerke in dem Gebiet können nicht durch die Bundesregierung erreicht werden. Derweil hat der Kanzler eine Mobilmachung der Bundeswehr angeordnet, die eine Versorgung des Gebietes erreichen soll. Aufnahmen von Wettersatelliten zeigen in der Region nichts go_on als eine ausnehmend großen Masse an dichten dunklen Wolken....\"\nIm nächsten Augenblick hatte das Radio auch die Deutsche Welle verloren. Der Wagen rollte derweil die Aachener Straße in Richtung Rheindahlen, Wegberg, Erkelenz entlang. Sie kamen am neuen Bundesliga-Stadion vorbei, doch die Sicht wurde now go_on, offener. Überall im Westen näherte sich eine riesige, bedrohliche Wolkenwand. Selbst im Süden konnte man now sehen, daß sie auch dort waren. Im Osten hingegen kam nun ein kleiner Spalt hellblauen Himmels zum Vorschein. Elke fuhr mit dem Auto auf einen Wendeplatz und hielt den Wagen an.\n\"Was denkst du, wohin sollen wir now fahren?\"");
	choice("Wenn du go_on nach Süden willst (1), nach Westen, wo die Wolken noch viel näher und finsterer sind (2), Osten, wo ein Streifen blauer Himmel now sichtbar wird (3), oder zurück nach Mönchengladbach, wo die Turmmonster sind, du dich aber auskennst (4).", 4, place106, place107, place108, place109, NULL, NULL);
}

void place101(void) {
// Du hast ein mulmiges Gefühl und versuchtst Elke vom rausfahren abzuhalten
	room = 101;
	if(tripodgesehen == true) {
		textoutput("Du erzählst Elke alles, was du gesehen hast, und beschreibst ihr verbal dieses turmgroße Etwas, das sich wie auf Stelzen zu bewegen schien und von dem aus Lichtbögen die Umgebung zerstörten.\nElke sieht dich irritiert an und fragt schließlich, wieso du ihr das nicht schon früher gesagt hättest? Schließlich bricht sie in schallendes Gelächter aus. Es vergeht eine Minute, bis sie sich wieder unter Kontrolle hat.\n\"Ich denke, du bist ein echt lieber Kerl, aber auch ein ziemlicher Phantast, hm?\"\nSie wirft den Motor an. Du glaubst deiner Erinnerung auch nicht wirklich - und irgendwie scheint es sowieso egal zu sein. Im Moment ist einzig und allein am wichtigsten, einmal etwas über die aktuelle Lage in Erfahrung zu bringen. Langsam rollt der Wagen durch das Parkhaus.");
	}
	else {
		textoutput("Du erzählst ihr, das du ein ungutes Gefühl hast, ob es nun an dem schlechten Wetter, den Sirenen oder was auch immer liegt, kannst du nicht sagen, aber du bringst klar zum Ausdruck, daß du kein gutes Gefühl dabei hast, in einem Auto auf die Straßen zu fahren, nicht now, nicht bei diesem Wetter, der Situation - und erst recht nicht, wenn es nach deinem Bauchgefühl geht. Aber Elkes Einwände sind besser, immerhind könntet ihr ein wenig erfahren, wieso die ganze Stadt einen Stromausfall erfahren hat. Du zuckst noch einmal mit den Schultern und willigst ein, schnallst dich an und schon geht es aus der Parklücke heraus und ihr rollt durch das dunkle Parkhaus.");
	}
	textoutput("Plötzlich gibt es so etwas wie einen kurzen, heftigen Erdstoß. Von einigen wenigen Autos schaltet sich die Diebstahlwarnanlage and und eröffnet eine neue Kakophonie innerhalb des Parkhauses. Jetz wäre dir wirklich nichts lieber, als endlich hier heraus zu sein. Ihr nähert euch der Ausfahrt. Elke schiebt ihre Karte in den alten Scheidt&Bachmann, dann öffnet sich die Schranke. Ihr fahrt langsam an - und verlaßt das Parkhaus. Elke will gerade abfahren, als zum zweiten Mal ein Erdstoß erfolgt.\n\"Gib bitte Gas!\" sagst du zu ihr, mit einem mummeligen Gefühl im Magen. Elke nickt und fährt los.");
	tryyourluck(place103, place102);
}

void place102(void) {
// Kein Glück - es hat Elke erwischt
	room = 102;
	textoutput("Mit einem fürchterlichem Geräusch von schneidendem Metall, einer unglaublichen Erschütterung und einem unbeschreiblichem, donnernden Geräusch, wird ein Teil eures Autos zerteten, durchgeteilt, vernichtet. Ein riesiger Metallfuß, wie du ihn von AT-AT aus dem Kino kennst, hat einen Teil des Motorblocks und die Fahrerseite zerquetscht. Du selber bist auch etwas eingequetscht. Dein Herz rast vor Angst und entsetzen. Neben dir, daß weißt du instinktiv, hat gerade Elke ihren Tod gefunden. Obwohl du sie kaum kanntest, tut dir das in der Seele weh, du weinst - versuchst aber gleichzeitig keinen Mucks zu machen - um nicht das gleiche Schicksal zu erleiden. In deiner verzweifelten Lage klappst du den Sitz langsam nach hinten. Du schaffst es, die Befestigung des Rücksitzes zu lösen - und kannst so deinen Rucksack aus dem Kofferraum holen. Du holst zusätzlich noch die Beutel die du erreichen kannst und stopfst dir deinen Rucksack damit voll. Du versuchst nicht zur Seite zu schauen, dir ein Bild von Elke zu machen.");
	choice("Willst du dich go_on, so leise wie möglich verhalten (1) oder willst du versuchen, aus dem Auto herauszukommen - und zu fliehen. Immerhin könnte der riesige Fuß ja noch ein zweites Mal auf das Auto herabsausen (2)?", 2, place104, place105, NULL, NULL, NULL, NULL);
}

void place103(void) {
// Glück gehabt
	room = 103;
	textoutput("Keine zwei Meter von eurem Auto entfernt, knallt ein riesiger Metallfuß mit ohrenbetäubendem Lärm und unter einer kurzen, heftigen Erschütterung, auf den Boden. Ihr werdet beide blass - doch sofort brüllst du Elke an: \"Fahr! Fahr! Fahr!\"\nSie löst sich aus ihrer Schockstarre und tritt das Gaspedal bis zum Anschlag durch. Mit einem Satz rollt ihr die Stepgesstraße hinab. Ohne auf die Ampelanlage zu achten, schießt ihr über die Kreuzung, am Kaiserbad vorbei in Richtung Korschenbroicher Straße, mit quietschenden Reifen biegt ihr in die Fliethstraße ein, während Elke immer go_on beschleunigt. Und du hast nichts dagegen einzuwenden, denn auch du willst nichts go_on, als weg. Ihr rast am Geroweiher vorbei, als du brüllst, sie soll nach Süden abbiegen. So rast ihr nahezu unaufhalsam über die Kreuzung und dann die Aachener Straße in entlang in Richtung Rheindalen. Für den Moment überlegst du ob ihr auf die Autobahn sollt, aber von dort aus kann man gar nicht mehr abbiegen.\n\"Nicht auf die Autobahn\", sagst du zu ihr, \"da säßen wir in der Falle - und unser Weg wäre vorhersehbar. Als ihr über die Brücke fahrt, die über die Autobahn führt, siehst du, wie klug eure Entscheidung war. Ein weiteres dieser Turmmonster steht mitten auf der Autobahn, einen Berg aus Blechkadavern zu seinen Füßen.\n\"Wohin fahren wir now?\" will sie wissen, während ihr now der Straße go_on in Richtung Wegberg und Erkelenz folgt.\n\"Erst mal go_on in Richtung Süden, an dieser Unwetterwolke und den Metalltürmen vorbei. Da entdeckt ihr weit im Süden, das die Wolke sich auch dort bereits findet. Elke fährt auf den nächsten Wendeplatz und hält den Wagen an. \"Bist du dir sicher, das wir go_on nach Süden fahren wollen?\"");
	choice("Wenn du go_on nach Süden willst (1), nach Westen, wo die Wolken noch viel näher und finsterer sind (2), Osten, wo ein Streifen blauer Himmel now sichtbar wird (3), oder zurück nach Mönchengladbach, wo die Turmmonster sind, du dich aber auskennst (4).", 4, place106, place107, place108, place109, NULL, NULL);
}

void place104(void) {
// Im Auto bleiben
	textoutput("Du verhältst dich muskmäuschen still. Den Rucksack hast du auf dem Schoß. Du atmest nur gan flach, schluchzt aber auch immer wieder. Du bedauerst deine eigene verzweifelte Lage noch viel mehr, als den Tod der freundlichen Frau, die hier, nur wenige Zentimeter entfernt von dir, unter einem tonnenschweren Stahlfuß zerquetscht wurde. Plötzlich hörst rechts neben dir ein Geräusch. Erschreckt siehst du zur Seite - und starrst in die Mündung eines Automatikgewehres.\nBefriedigt grunzt das Wesen, das gerade seine Waffe in deinem Kopf entladen hat, dann wendet es sich vom Anblick deines Kadavers ab und sucht nach weiteren \"Ausreißern\".");
	exit(EXIT_SUCCESS);
}

void place105(void) {
// escape
	room = 105;
}

void place106(void) {
// Süden
	room = 106;
}

void place107(void) {
// Westen
	room = 107;
}

void place108(void) {
// Osten
	room = 108;
}

void place109(void) {
// Norden
	room = 109;
}

void place110(void) {
}

void place111(void) {
}

void place112(void) {
}

void place113(void) {
}

void place114(void) {
}

void place115(void) {
}

void place116(void) {
}

void place117(void) {
}

void place118(void) {
}

void place119(void) {
}

void place120(void) {
}

void place121(void) {
}

void place122(void) {
}

void place123(void) {
}

void place124(void) {
}

void place125(void) {
}

void place126(void) {
}

void place127(void) {
}

void place128(void) {
}

void place129(void) {
}

void place130(void) {
}

void place131(void) {
}

void place132(void) {
}

void place133(void) {
}

void place134(void) {
}

void place135(void) {
}

void place136(void) {
}

void place137(void) {
}

void place138(void) {
}

void place139(void) {
}

void place140(void) {
}

void place141(void) {
}

void place142(void) {
}

void place143(void) {
}

void place144(void) {
}

void place145(void) {
}

void place146(void) {
}

void place147(void) {
}

void place148(void) {
}

void place149(void) {
}

void place150(void) {
}

void place151(void) {
}

void place152(void) {
}

void place153(void) {
}

void place154(void) {
}

void place155(void) {
}

void place156(void) {
}

void place157(void) {
}

void place158(void) {
}

void place159(void) {
}

void place160(void) {
}

void place161(void) {
}

void place162(void) {
}

void place163(void) {
}

void place164(void) {
}

void place165(void) {
}

void place166(void) {
}

void place167(void) {
}

void place168(void) {
}

void place169(void) {
}

void place170(void) {
}

void place171(void) {
}

void place172(void) {
}

void place173(void) {
}

void place174(void) {
}

void place175(void) {
}

void place176(void) {
}

void place177(void) {
}

void place178(void) {
}

void place179(void) {
}

void place180(void) {
}

void place181(void) {
}

void place182(void) {
}

void place183(void) {
}

void place184(void) {
}

void place185(void) {
}

void place186(void) {
}

void place187(void) {
}

void place188(void) {
}

void place189(void) {
}

void place190(void) {
}

void place191(void) {
}

void place192(void) {
}

void place193(void) {
}

void place194(void) {
}

void place195(void) {
}

void place196(void) {
}

void place197(void) {
}

void place198(void) {
}

void place199(void) {
}

// -----------
// --- 200 ---
// -----------

void place200(void) {
}

void place201(void) {
}

void place202(void) {
}

void place203(void) {
}

void place204(void) {
}

void place205(void) {
}

void place206(void) {
}

void place207(void) {
}

void place208(void) {
}

void place209(void) {
}

void place210(void) {
}

void place211(void) {
}

void place212(void) {
}

void place213(void) {
}

void place214(void) {
}

void place215(void) {
}

void place216(void) {
}

void place217(void) {
}

void place218(void) {
}

void place219(void) {
}

void place220(void) {
}

void place221(void) {
}

void place222(void) {
}

void place223(void) {
}

void place224(void) {
}

void place225(void) {
}

void place226(void) {
}

void place227(void) {
}

void place228(void) {
}

void place229(void) {
}

void place230(void) {
}

void place231(void) {
}

void place232(void) {
}

void place233(void) {
}

void place234(void) {
}

void place235(void) {
}

void place236(void) {
}

void place237(void) {
}

void place238(void) {
}

void place239(void) {
}

void place240(void) {
}

void place241(void) {
}

void place242(void) {
}

void place243(void) {
}

void place244(void) {
}

void place245(void) {
}

void place246(void) {
}

void place247(void) {
}

void place248(void) {
}

void place249(void) {
}

void place250(void) {
}

void place251(void) {
}

void place252(void) {
}

void place253(void) {
}

void place254(void) {
}

void place255(void) {
}

void place256(void) {
}

void place257(void) {
}

void place258(void) {
}

void place259(void) {
}

void place260(void) {
}

void place261(void) {
}

void place262(void) {
}

void place263(void) {
}

void place264(void) {
}

void place265(void) {
}

void place266(void) {
}

void place267(void) {
}

void place268(void) {
}

void place269(void) {
}

void place270(void) {
}

void place271(void) {
}

void place272(void) {
}

void place273(void) {
}

void place274(void) {
}

void place275(void) {
}

void place276(void) {
}

void place277(void) {
}

void place278(void) {
}

void place279(void) {
}

void place280(void) {
}

void place281(void) {
}

void place282(void) {
}

void place283(void) {
}

void place284(void) {
}

void place285(void) {
}

void place286(void) {
}

void place287(void) {
}

void place288(void) {
}

void place289(void) {
}

void place290(void) {
}

void place291(void) {
}

void place292(void) {
}

void place293(void) {
}

void place294(void) {
}

void place295(void) {
}

void place296(void) {
}

void place297(void) {
}

void place298(void) {
}

void place299(void) {
}

// -----------
// --- 300 ---
// -----------

void place300(void) {
}

void place301(void) {
}

void place302(void) {
}

void place303(void) {
}

void place304(void) {
}

void place305(void) {
}

void place306(void) {
}

void place307(void) {
}

void place308(void) {
}

void place309(void) {
}

void place310(void) {
}

void place311(void) {
}

void place312(void) {
}

void place313(void) {
}

void place314(void) {
}

void place315(void) {
}

void place316(void) {
}

void place317(void) {
}

void place318(void) {
}

void place319(void) {
}

void place320(void) {
}

void place321(void) {
}

void place322(void) {
}

void place323(void) {
}

void place324(void) {
}

void place325(void) {
}

void place326(void) {
}

void place327(void) {
}

void place328(void) {
}

void place329(void) {
}

void place330(void) {
}

void place331(void) {
}

void place332(void) {
}

void place333(void) {
}

void place334(void) {
}

void place335(void) {
}

void place336(void) {
}

void place337(void) {
}

void place338(void) {
}

void place339(void) {
}

void place340(void) {
}

void place341(void) {
}

void place342(void) {
}

void place343(void) {
}

void place344(void) {
}

void place345(void) {
}

void place346(void) {
}

void place347(void) {
}

void place348(void) {
}

void place349(void) {
}

void place350(void) {
}

void place351(void) {
}

void place352(void) {
}

void place353(void) {
}

void place354(void) {
}

void place355(void) {
}

void place356(void) {
}

void place357(void) {
}

void place358(void) {
}

void place359(void) {
}

void place360(void) {
}

void place361(void) {
}

void place362(void) {
}

void place363(void) {
}

void place364(void) {
}

void place365(void) {
}

void place366(void) {
}

void place367(void) {
}

void place368(void) {
}

void place369(void) {
}

void place370(void) {
}

void place371(void) {
}

void place372(void) {
}

void place373(void) {
}

void place374(void) {
}

void place375(void) {
}

void place376(void) {
}

void place377(void) {
}

void place378(void) {
}

void place379(void) {
}

void place380(void) {
}

void place381(void) {
}

void place382(void) {
}

void place383(void) {
}

void place384(void) {
}

void place385(void) {
}

void place386(void) {
}

void place387(void) {
}

void place388(void) {
}

void place389(void) {
}

void place390(void) {
}

void place391(void) {
}

void place392(void) {
}

void place393(void) {
}

void place394(void) {
}

void place395(void) {
}

void place396(void) {
}

void place397(void) {
}

void place398(void) {
}

void place399(void) {
}

void place400(void) {
}

// ---------------
// The mainroutine
// ---------------
int main(void) {
	time_t now;
	bool playerisalive = true;

	// initialize randomnumber
	now = time((time_t *) NULL);
	srand((unsigned) now);

	// set environment variables for localisation
	setlocale(LC_ALL, "");

	initscr(); // begin ncurses
	atexit(quit);
	keypad(stdscr, true); // activate keymappin
	cbreak(); // don't wait for enter on input
	echo(); // cursor-echo
	scrollok(stdscr, true); // deactivate automatic scrolling of the output - we do it per page
	start_color(); // start with colorn
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	clear(); // clear the whole window
	curs_set(0);
	// atexit(quit); // Routine, for implementing,  what is done on exit - not used yet

	color_set(2, 0); // Blue characters on black background
	textoutput("------------------------------");
	textoutput("Steampunk FFR - How it started");
	textoutput("------------------------------");
	color_set(0, 0);
	textoutput("A \"This-is-your-Adventure\" novel\n");
	color_set(2, 0);
	textoutput("Based on a story by Sascha Biermanns\n");
	color_set(0, 0);
	if(yesnoquestion("Would you like to go on with a previously saved game (y/n)?"))
		load();
	intro();
	vorwort();
	place1();
}

// Implementation x-sided dice
int dice(unsigned int maximumnumber) {
	// from 0..5 you get 1..6
	return (rand() % (maximumnumber - 1)) + 1;
}

// Implementation Test your luck
bool tyl(character_t *figur) {
	if(figur->luck <= 0)

		return false; // No more luck for you, so it's unlucky!
	if((dice(6) + dice(6)) <= figur->luck--)
		return true; // that was lucky
	return false; // while this one wasn't
}

// Implementation: fightround
bool fightround(character_t *attacker, character_t *defender, void (*pointtoescape)()) {
	int testdexterity[2];
	bool lucktest = false;
	bool reallucky;
	int shieldbonus = 0;

	testdexterity[0] = attacker->dexterity + dice(6) + dice(6) + attackbonus + paralized;
	testdexterity[1] = defender->dexterity + dice(6) + dice(6);
	if(invisible) // in case,  you are invisible
		testdexterity[0] += 2;
	if(testdexterity[0] == testdexterity[1])
		return true; // on equal results - it's a not hit
	actual_values(attacker);
	actual_values(defender);
	// Do you want to try your flight?
	if(pointtoescape != NULL)
		if(yesnoquestion("Do you plan to escape (y/n)? "))
			escape(pointtoescape);
	// Do you want to try your luck?
	if(attacker->luck > 0) {
		if(yesnoquestion("Would you like to test you're luck (y/n)? ")) {
			lucktest = true;
			reallucky = tyl(attacker);
		}
	}
	// Here starts the real fightpart
	if(testdexterity[0] > testdexterity[1]) { // The player wins
		if((onlysilverhits == true) && (silverweapon == false)) // silver immune monster
			return true; // don't get hits
		if(invisible) // Invisible player
			defender->strength -= 2;
		if(((object[riffle] > 0) || (object[gun] > 0)) && (object[ammunition] > 0)) { // gun = + 1 point, as long as you have ammunition
			defender->strength -= 1;
			object[ammunition] -= 1;
		}
		if(lucktest) {
			if(reallucky)
				defender->strength -= 4;
			else
				defender->strength -= 1;
			return true; // Lucky,  no hit
		}
		defender->strength -= 2;
		return true; // Lucky,  no hit
	}
	else if (testdexterity[0] < testdexterity[1]) { // The player loses this round
		if((object[schild] >= 0) && (dice(6) == 6)) // Luckily,  the shield get's the hit
			shieldbonus = 1;
		if(invisible) // The bonus for invisibility
			switch(dice(6)) {
				case 2:
				case 4:
					attacker->strength += 1;
					break;
				case 6:
					return true; // No hit,  thanks to the invisibility
				default:
					break;
			}
		if(lucktest) {
			if(reallucky)
				attacker->strength -= 1 - shieldbonus;
			else
				attacker->strength -= 4 - shieldbonus;
			return false; // It was a hit
		}
		attacker->strength -= 2 - shieldbonus;
		return false; // It was a hit
	}
	return true; // No winner - no hit
}

// Implementation: fight
bool fight(character_t *player, character_t *enemy, int anzahl, bool dontgethit, void (*pointtoescape)()) {
	int totalstrength = 0;
	bool nohitlanded = true;

	for(int i=0; i < anzahl; i++)
		totalstrength += enemy[i].strength;
	while((player->strength > 0) && (totalstrength > 0)) {
		for(int i=0; i < anzahl; i++)
			if(player->strength > 0) {
				if(enemy[i].strength > 0)
					nohitlanded = fightround(player, &enemy[i], pointtoescape);
				if(dontgethit)
					if(!nohitlanded)
						return false;
			}
			else
				return false;
		// Is the enemy still alive?
		totalstrength = 0;
		for(int i=0; i < anzahl; i++) {
			if(enemy[i].strength < 0)
				enemy[i].strength = 0;
			totalstrength += enemy[i].strength;
		}
	}
	if(player->strength > 0)
		return true; // The player is alive ^.^
	return false; // The player is dead v.v
}

// Implementation: Momentane Werte
void actual_values(character_t *person) {
	printw("\n     Name: %s\n", person->name);
	printw("Dexterity: %2d / %2d\n", person->dexterity, person->dexterity_start);
	printw(" Strength: %2d / %2d\n", person->strength, person->strength_start);
	printw("     Luck: %2d / %2d\n", person->luck, person->luck_start);
}

// Implementation: key
char key(void) {
	int buffer;
	char zeichen;

	noecho();
	zeichen = getch();
	echo();
	return(zeichen);
}

// Implementation: textoutput
void textoutput(char *completetext) {
	int linelength = COLS; // COLS ist eine ncurses Variable
	int maximumlines = LINES; // LINES ist eine ncurses Variable

	char *resttext = completetext;
	char textline[linelength];
	int i;
	int j;
	int line = 0;
	bool firstoutput = true;
	int x, y;

	for(i = 0; i < linelength; i++)
		textline[i] = '\0'; // Safetydeletion - otherwise you have problems with empty lines
	while(strlen(resttext) > (linelength - 1)) {
		for(i = (linelength - 1); (*(resttext+i) != ' ') && (i > 0); i--);
		if(!i)
			i = (linelength - 1); // The damned word is longer then the line
		for(j = 0; (*(resttext+j) != '\n') && (j < i); j++);
		if(j < i)
			i = j; // Shrink to the end of line
		strncpy(textline, resttext, i); // copy the textpart
		resttext += i+1;
		while(*resttext == ' ')
			resttext++;
		// Check if we are in the 2nd last line
	  	getyx(stdscr, y, x); // get the cursorposition in the window
		if((firstoutput == true) && (y >= (maximumlines - 1))) {
			go_on();
			line = 0;
			firstoutput = false;
		}
		if(line == (maximumlines - 1)) {
				go_on();
				line = 0;
		}
		printw("%s\n", textline);
		for(i = 0; i < linelength; i++)
			textline[i] = '\0'; // Safetydeletion - otherwise you have problems with empty lines
		line += 1;
	}
	// Text ist kürzer als eine line.
	// Prüfen, ob wir in der vorletzten line angekommen sind
/*  getyx(stdscr, y, x); // Cursorposition feststellen
	if((firstoutput == true) && (y >= (maximumlines - 2))) {
		go_on();
		line = 0;
		firstoutput = false;
	}
*/
	if(line == (maximumlines - 1)) {
			go_on();
			line = 0;
	}
	printw("%s\n", resttext);
	refresh();
}

// Implementation: go_on
void go_on(void) {
	printw("--- Plese press ENTER key to go on with the story --- ");
	getch();
	clear(); // Erase screen - output will start at the topmost point of the window again
	curs_set(0);
}

// Implementation: choice
void choice(char *description, int maxzahl, void (*funktion1)(), void (*funktion2)(), void (*funktion3)(), void (*funktion4)(), void (*funktion5)(), void (*funktion6)()) {
	char input[20];
	int result = 0;
	char additionaldescription[300];

	while((result < 1) || (result > maxzahl)) {
		color_set(1, 0);
		strcpy(additionaldescription, " ");
		if(object[dexteritypotion] > 0)
			strcat(additionaldescription, "(7) drink potion of dexterity ");
		if(object[strengthpotion] > 0)
			strcat(additionaldescription, "(8) drink potion of strength ");
		if(object[luckpotion] > 0)
			strcat(additionaldescription, "(9) drink potion of luck ");
			strcat(additionaldescription, "(77) load last saved position (88) save actual position (99) save actual position & quit");
		textoutput(description);
		textoutput(additionaldescription);
		getnstr(input, 20);
		result = atoi(input);
		color_set(0, 0);
		switch(result) {
			case 7: drink_dexteritypotion();
					break;
			case 8: drink_strengthpotion();
					break;
			case 9: drink_luckpotion();
					break;
			case 77: load();
					 break;
			case 88: save();
					 break;
			case 99: save();
					 color_set(2, 0);
					 printw("See you next time!\n");
					 color_set(0, 0);
					 refresh();
					 exit(EXIT_SUCCESS);
			default: break;
		}
	}
	switch(result) {
		case 1: funktion1();
				break;
				;;
		case 2: funktion2();
				break;
				;;
		case 3: funktion3();
				break;
				;;
		case 4: funktion4();
				break;
				;;
		case 5: funktion5();
				break;
				;;
		case 6: funktion6();
				break;
				;;
		default: fputs("ERROR! The variable result in function choice() had a wrong value.", stdin);
				 exit(EXIT_FAILURE);
	}
}

// Funktion: tryyourluck
void tryyourluck(void (*funktion1)(), void (*funktion2)()) {
	color_set(1, 0);
	textoutput("--- Try your luck! - press ENTER ---");
//	getchar();
	getch();
	color_set(0, 0);
	if(tyl(&player)) // Lucky you
		funktion1();
	else 			// no luck - let's see what comes
		funktion2();
}

// Funktion: tryyourdexterity
void tryyourdexterity(void (*funktion1)(), void (*funktion2)()) {
	bool dexterity;

	color_set(1, 0);
	textoutput("--- Try your dexterity! - press ENTER ---");
	getch();
	color_set(0, 0);
	dexterity = dice(6) + dice(6);
	printw("You're actual dexterity is %d, you're roll of dice is %d\n", player.dexterity, dexterity);
	if(dexterity <= player.dexterity) // Well done
		funktion1();
	else 			// Not so good - see what comes in this case
		funktion2();
}

// Implementation: choose
int choose(char* description, int maxzahl) {
	int result;
	char input[20];

	color_set(1, 0);
	while((result < 1) || (result > maxzahl)) {
		textoutput(description);
		getnstr(input, 20);
		result = atoi(input);
	}
	color_set(0, 0);
	return result;
}

// Funktion: escape
void escape(void (*funktion1)()) {
	textoutput("You decide to try and escape,  what will cost you 2 points of strength.");
	if(player.luck > 0) { // Is a test of luck possible?
		textoutput("Of cause,  you could try you're luck. If you're lucky, it could be a lucky escape - and you loose only 1 point of strength. But if you're not lucky,  you're escape could turn into a desaster - and cost you 3 points of strength.");
		if(yesnoquestion("Would you like to try you're luck (y/n)? ")) {
			if(tyl(&player)) // Glück gehabt
				player.strength -= 1;
			else // Pech gehabt
				player.strength -= 3;
		}
		else
			player.strength -= 2;
	}
	else {
		player.strength -= 2;
	}
	if(player.strength) {
		funktion1();
		color_set(4, 0);
		printw("ERROR! It looks,  the place() is still empty.");
		exit(EXIT_FAILURE);
	}
	else {
		textoutput("I'm so sorry for you,  dear. It looks like you are so exhausted,  you even can't take another breath. You feel real pain,  while the air escapes from you're lungs. Then everything starts to become red before you're eyes,  and you softly seem to wander into a black void. This is you're END.");
		exit(EXIT_SUCCESS);
	}
	color_set(4, 0);
	printw("ERROR! I'm back in the escape function - after returning from a place() that is not well implemented. Last known roomnumber is %di\n", room);
	exit(EXIT_FAILURE);
}

// Implementation: haveameal
void haveameal(void) {
	if(yesnoquestion("Would you like to have a meal (y/n)? "))
		if(object[proviant] > 0) {
			textoutput("After securing you're actual position,  you take some food out of you're backpack. You chew on you're meal - and it tastes delicious. You can feel, how you're strength comes back - and gives new life to you're tired body.");
			object[proviant] -= 1;
			raisestrength(4, 0);
		}
		else
			textoutput("After securing you're actual position,  you reach into you're backpack, but can't find anything to eat. You search minute for minute,  but nothing. You'll have to go on with an empty stomach.");
}

// Funktion: dexteritypotion trinken
void drink_dexteritypotion() {
	if(object[dexteritypotion] > 0) {
		object[dexteritypotion] -= 1;
		player.dexterity = player.dexterity_start;
		textoutput("You drink the potion of dexterity. You're dexterity couldn't be better.");
	}
}

// Funktion: Stärketrank trinken
void drink_strengthpotion() {
	if(object[strengthpotion]) {
		object[strengthpotion] -= 1;
		player.strength = player.strength_start;
		textoutput("You drink the potion of strength. You're strength couldn't be better.");
	}
}

// Funktion Glückstrank trinken
void drink_luckpotion() {
	if(object[luckpotion]) {
		object[luckpotion] -= 1;
		player.luck_start += 1;
		player.luck = player.luck_start;
		textoutput("You drink the potion of luck. You feel much more lucky then ever before.");
	}
}

// Implementation: Objkekt ablegen
void drop_object(void) {
	char input[21];
	int result = -1;
	char bestaetigung;
	int j=0;

	for(int i=0; (i < maximumobject) && (object[i] > 0); i++) {
		printw("(%d) %s ", object[i], objectname[i]);
		j++;
		if(!(j % 3))
			printw("\n");
	}
	while((result < 0) || (result >= maximumobject)) {
		textoutput("Please enter the given number of the object! ");
		getnstr(input, 20);
		result = atoi(input);
	}
	color_set(1, 0);
	printw("Really drop %s? ", objectname[result]);
	bestaetigung = key();
	color_set(0, 0);
	if((bestaetigung == 'y') || (bestaetigung == 'Y')) {
	object[result] -= 1;
		printw("\n%s dropped.\n", objectname[result]);
	}
}

// Implementation: Ja-Nein-Frage
bool yesnoquestion(char *frage) {
	char input;
	color_set(1, 0);
	textoutput(frage);
	input = key();
	color_set(0, 0);
	if((input == 'y') || (input == 'Y'))
		return true;
	else
		return false;
}

// Implementation: raisedexterity
void raisedexterity(int temporaer, int permanent) {
	player.dexterity_start += permanent;
	player.dexterity += temporaer;
	if(player.dexterity > player.dexterity_start)
		player.dexterity = player.dexterity_start;
}

// Implementation: raisestrength
void raisestrength(int temporaer, int permanent) {
	player.strength_start += permanent;
	player.strength+= temporaer;
	if(player.strength > player.strength_start)
		player.strength = player.strength_start;
}

// Implementation: raiseluck
void raiseluck(int temporaer, int permanent) {
	player.luck_start += permanent;
	player.luck += temporaer;
	if(player.luck > player.luck_start)
		player.luck = player.luck_start;
}

// -------------------------
// Implementation: save
// -------------------------

int save(void) {
	FILE *datei;
	datei = fopen(FILENAME, "w");
	if(ferror(datei)) {
		color_set(4, 0);
		printw("The file can't be saved. I'm sorry, but you still can go on without having saved.");
		color_set(0, 0);
		return 1;
	}

	fprintf(datei, "%s\n", player.name);
	fprintf(datei, "%d\n", player.dexterity);
	fprintf(datei, "%d\n", player.dexterity_start);
	fprintf(datei, "%d\n", player.luck);
	fprintf(datei, "%d\n", player.luck_start);
	fprintf(datei, "%d\n", player.strength);
	fprintf(datei, "%d\n", player.strength_start);
	fprintf(datei, "%d\n", room);
	for(int i=0; i<maximumobject; i++)
		fprintf(datei, "%d\n", object[i]);
	fprintf(datei, "%d\n", attackbonus);
	fprintf(datei, "%d\n", fuenfwahl);
	fprintf(datei, "%d\n", (int) onlysilverhits);
	fprintf(datei, "%d\n", paralized);
	fprintf(datei, "%d\n", price);
	fprintf(datei, "%d\n", schluessel);
	fprintf(datei, "%d\n", (int) schluessel9);
	fprintf(datei, "%d\n", (int) schluessel66);
	fprintf(datei, "%d\n", (int) schluessel99);
	fprintf(datei, "%d\n", (int) schluessel111_1);
	fprintf(datei, "%d\n", (int) schluessel111_2);
	fprintf(datei, "%d\n", (int) schluessel125);
	fprintf(datei, "%d\n", (int) schluesselbootshaus);
	fprintf(datei, "%d\n", (int) cheating);
	fprintf(datei, "%d\n", (int) silverweapon);
	fprintf(datei, "%d\n", (int) invisible);
	fprintf(datei, "%d\n", (int) tripodgesehen);
	fprintf(datei, "%d\n", killedpeople);
	fprintf(datei, "%d\n", killedfoes);
	fprintf(datei, "%d\n", (int) agartha);
	fprintf(datei, "%d\n", (int) verzeichnisgelesen);
	fprintf(datei, "%d\n", (int) buchgefunden);
	fprintf(datei, "%d\n", (int) kartegefunden);
	fprintf(datei, "%d\n", (int) sargverschoben);
	fprintf(datei, "%d\n", (int) durchganggeoeffnet);
	fprintf(datei, "%d\n", (int) schluesselgefunden);
	fclose(datei);

	color_set(3, 0);
	printw("The game was saved.\n");
	printw("room: %d\n", room);
	color_set(0, 0);
	return 0;
}


// -------------------------
// Implementation: load
// -------------------------

int load(void) {
	char input[100];
	FILE *datei;
	datei = fopen(FILENAME, "r");
	if(ferror(datei)) {
		color_set(4, 0);
		printw("The file couldn't be loaded. You can go on without the loaded saved game.");
		color_set(0, 0);
		return 1;
	}
	fgets(input, 100, datei);
	for(int i=0; i < strlen(input); i++)
		if(input[i] == '\n')
			input[i] = '\0';
	strcpy(player.name, input);
	fgets(input, 100, datei);
	player.dexterity = atoi(input);
	fgets(input, 100, datei);
	player.dexterity_start = atoi(input);
	fgets(input, 100, datei);
	player.luck = atoi(input);
	fgets(input, 100, datei);
	player.luck_start = atoi(input);
	fgets(input, 100, datei);
	player.strength = atoi(input);
	fgets(input, 100, datei);
	player.strength_start = atoi(input);
	fgets(input, 100, datei);
	room = atoi(input);
	for(int i=0; i<maximumobject; i++) {
		fgets(input, 100, datei);
		object[i] = atoi(input);
	}
	fgets(input, 100, datei);
	attackbonus = atoi(input);
	fgets(input, 100, datei);
	fuenfwahl = atoi(input);
	fgets(input, 100, datei);
	onlysilverhits = (bool) atoi(input);
	fgets(input, 100, datei);
	paralized = atoi(input);
	fgets(input, 100, datei);
	price = atoi(input);
	fgets(input, 100, datei);
	schluessel = atoi(input);
	fgets(input, 100, datei);
	schluessel9 = (bool) atoi(input);
	fgets(input, 100, datei);
	schluessel66 = (bool) atoi(input);
	fgets(input, 100, datei);
	schluessel99 = (bool) atoi(input);
	fgets(input, 100, datei);
	schluessel111_1 = (bool) atoi(input);
	fgets(input, 100, datei);
	schluessel111_2 = (bool) atoi(input);
	fgets(input, 100, datei);
	schluessel125 = (bool) atoi(input);
	fgets(input, 100, datei);
	schluesselbootshaus = (bool) atoi(input);
	fgets(input, 100, datei);
	cheating = (bool) atoi(input);
	fgets(input, 100, datei);
	silverweapon = (bool) atoi(input);
	fgets(input, 100, datei);
	invisible = (bool) atoi(input);
	fgets(input, 100, datei);
	tripodgesehen = (bool) atoi(input);
	fgets(input, 100, datei);
	killedpeople = atoi(input);
	fgets(input, 100, datei);
	killedfoes = atoi(input);
	fgets(input, 100, datei);
	agartha = (bool) atoi(input);
	fgets(input, 100, datei);
	verzeichnisgelesen = (bool) atoi(input);
	fgets(input, 100, datei);
	buchgefunden = (bool) atoi(input);
	fgets(input, 100, datei);
	kartegefunden = (bool) atoi(input);
	fgets(input, 100, datei);
	sargverschoben = (bool) atoi(input);
	fgets(input, 100, datei);
	durchganggeoeffnet = (bool) atoi(input);
	fgets(input, 100, datei);
	schluesselgefunden = (bool) atoi(input);
	fclose(datei);

	color_set(3, 0);
	printw("Spielstand geladen.\n");
	actual_values(&player);
	printw("room: %d\n", room);
	color_set(0, 0);
	raumptr[room](); // go_on geht's im Spiel in Raum [raum]
	return 0;
}

// Funktion: quit
void quit(void) {
	endwin();
}
