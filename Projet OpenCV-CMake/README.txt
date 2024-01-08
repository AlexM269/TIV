CONTEXTE DU PROJET :

Le but du projet est de concevoir une application de reconnaissance de formes représentant des symboles d’urgence dessinés à la main.
L’application doit être mise en œuvre à travers les trois étapes suivantes :

- Prétraitement et traitement d'images
- Extraction de fonctionnalités
- Classification

Ce projet se base sur un ensemble de 14 icônes qui peuvent être utilisées par les services d'urgence en situation de crise. Ces icônes permettent de communiquer rapidement
sur la situation avec des tablettes, en transmettant des informations sur l'accident de manière visuelle et en demandant ainsi du matériel spécifique.

Nous souhaitons donc réaliser une méthode d'identification automatique de ces symboles qui vise à être le plus fonctionnelle possible, indépendamment de la manière de dessiner
des individus. Nous travaillons avec OpenCV. Notre solution sera ensuite évaluée afin de quantifier ses performances et de déterminer les cas d’échec.


Etape 1 - Prétraitement et traitement d'images :

Dans cette première partie, nous travaillons sur une base de données obtenue à partir de 35 scripteurs bénévoles ayant reproduit chacune des 14 icônes d’urgence, en 3 tailles
différentes (grande, moyenne, petite).

Chaque scripteur a rempli 22 formulaires. Sur un formulaire (dans lequel le scripteur le numéro est renseigné), chacune des 14 lignes contient :
- Un modèle imprimé de l'icône cible à reproduire
- Un ensemble de 5 cases où l'icône doit être reproduite à la main.

Pour chaque scripteur, les deux premiers formulaires (0 et 1) sont utilisés pour l'entraînement et contiennent chacun les 14 icônes, sans précision de taille.
Dans les formulaires numérotés de 2 à 21, les icônes sont classées aléatoirement tout comme la taille spécifiée en dessous. L'ordre est donc différent pour chaque scripteur.
Les derniers formulaires contiennent des écritures manuscrites.

L’objectif est d’établir un ensemble fiable à partir des données fournies en construisant une base de données contenant des dessins isolés et les informations associées : icône,
taille, scripteur, identifiant du formulaire, emplacement du formulaire.


FONCTIONNEMENT DU PROGRAMME :

Notre application se découpe en quatre fonctions principales dont nous allons détailler la méthodologie.

1 ) Identification des formulaires

Tout d’abord, nous utilisons les noms des fichiers pour identifier les formulaires. Après observation, nous avons constaté que la nomination des fichiers suivait toujours le
même format : les premiers chiffres identifient le scripteur et les deux derniers indiquent le numéro du formulaire. Nous récupérons donc ces données à chaque ouverture d’une
image et les utilisons pour nommer les imagettes finales.

2 ) Redressement des images

Pour pouvoir récupérer les informations souhaitées de manière identique pour tous les formulaires, nous en normalisons l’affichage. Pour ce faire, nous avons choisi de repérer
des éléments clés présents sur chaque image afin de les positionner à un emplacement fixe, en fonction d’une image de référence que nous avons choisie. Les éléments ciblés sont
les deux croix présentes dans le coin supérieur droit et inférieur gauche ainsi que le bandeau noir au-dessus des cases dans lesquelles les scripteurs ont dessiné. L’image de
référence retenue est la 00102 de la base de données d'entraînement que nous avions à notre disposition dès le début du projet.

Pour localiser les composantes intéressantes, nous utilisons la fonction du système matchTemplate() qui permet de repérer des éléments donnés dans une image. Nous avons dû
déterminer un seuil d’acceptation pour sélectionner les zones du scan qui correspondaient véritablement à ces objets. Nous l’avons ajusté en testant différentes valeurs sur un
ensemble d’images afin qu’il ne soit ni trop restrictif, ni trop permissif. Nous pouvons ensuite calculer le centre des zones repérées afin d’obtenir trois points distincts.

Après avoir récolté ces coordonnées caractéristiques, nous devons positionner les formulaires de la même manière que le fichier de référence. Pour cela, nous nous appuyons sur
une transformation affine qui permet de placer trois points renseignés à des coordonnées spécifiques sans modifier les parallèles de l’image. Nous utilisons donc cette méthode
en fournissant les points relevés à l’étape précédente comme coordonnées d’entrée et leurs positions analogues issues de l’image de référence comme coordonnées de sortie. Les
trois points n’étant pas colinéaires, cette technique redresse l’image et la fixe toujours dans la même disposition afin de faciliter l’extraction des informations suivantes.

3 ) Extraction des informations

Après avoir normalisé les positionnement des formulaires, chaque élément est supposé se trouver exactement aux mêmes coordonnées d’un document à l’autre. Nous extrayons donc
chaque composante de la première colonne à l’aide de translations verticales régulières. Ces composantes sont enregistrées dans des fichiers temporaires qui seront détruits à
la fin du programme.

Une première fonction se charge donc de découper les tailles inscrites sous les icônes. Les images qui en découlent sont ensuite comparées à 3 images modèles (correspondant à
chaque taille) stockées dans un autre dossier. La similarité est quantifiée à l’aide de la fonction du système matchShapes() associée à la méthode CONTOURS_MATCH_I1 qui s’est
avérée la plus performante. Le minimum est retenu et la ligne des icônes est affectée à la taille ayant donné cette valeur. Si le calcul de similarité donne un mauvais résultat
(nous avons fixé le seuil après plusieurs tests), nous considérons que la taille est illisible ou absente et aucune taille n’est affectée.

Suivant la même logique, une seconde fonction découpe les zones correspondant aux positions des icônes imprimées. Ici, les symboles sont plus complexes et reçoivent donc un
traitement supplémentaire. Nous les érodons et dilatons de sorte à combler les éventuels trous ou irrégularités variables selon la qualité des scans. Nous appliquons ensuite un
flou gaussien pour homogénéiser le résultat. Enfin, nous binarisons “à la main” chaque pixel de sorte à obtenir du noir ou du blanc total. Nous n’appliquons pas la fonction de
binarisation préexistante car les images doivent rester en niveaux de gris pour la suite du traitement. Les 14 modèles enregistrés subissent les mêmes ajustements puis sont
parcourus pour les comparer aux icônes extraites, tout comme pour la gestion des tailles. Chacune est donc étiquetée selon la meilleure similarité trouvée. Ici, la ligne est
abandonnée si le calcul donne un résultat considéré comme non fiable. En effet, il est possible qu’il s’agisse simplement d’un document ne présentant aucune icône, comme les
derniers fichiers fournis qui sont entièrement manuscrits. Par ailleurs, les tests ont montré qu’une similarité dépassant notre seuil était quoi qu’il en soit associée à une
affectation le plus souvent erronée.

4 ) Constitution des fichiers descriptifs et des images associées

Nous extrayons ensuite l’ensemble des icônes dessinées à la main en découpant les images selon une translation vers la droite puis vers le bas, à l’instar des extractions
temporaires précédentes. Cette translation a été calculée à partir d’un modèle redressé de sorte à concorder avec la position de chaque encadré où le symbole est reproduit par
le scripteur. Les imagettes sont ensuite enregistrées avec le format du titre imposé, en récupérant les informations issues des étapes précédentes et leur position dans le
formulaire (donnée par le rang de la boucle auquel elles ont été générées).

Enfin, nous recensons l’ensemble des informations extraites dans des fichiers textes respectant encore une fois la structure demandée. Chaque fichier correspond à une imagette
et est ainsi composé d’une courte description, de l’identification de son label et de sa taille déterminés plus tôt. Il comporte également son identifiant composé du numéro de
scripteur, du numéro de la page, de la ligne et de la colonne. Le contenu est enregistré par une fonction d’écriture de fichier texte classique.


RESULTATS ET PERFORMANCE :

Après avoir créé notre programme à partir des formulaires fournis au début du projet, nous avons donc pu évaluer à la main notre programme en le testant sur la base de test
fournie en aval et en vérifiant les résultats.

1 ) Quantification de la performance

L'identification des formulaires et des scripteurs se basant sur l’intitulé des documents, aucune source d’erreur n’est possible pour ces informations
(à moins d’une incohérence lors de l’enregistrement des formulaires).
Le taux de réussite est donc de 100%. Pour le reste, notre solution présente les taux suivants :

420/420 icônes dont la taille est correctement référencée
229/420 icônes dont le label est correctement étiqueté
401/420 imagettes correctement extraites (bien cadrées et sauvegardées)
223/420 imagettes correctement extraites et liées aux bonnes informations.

7 labels fire brigad sont identifiés comme flood.
5 labels injury sont identifiés comme électricité.
4 labels car sont identifiés comme fire brigad.
4 labels fire sont identifiés comme casualty.
3 labels gas sont identifiés comme fire.
3 labels person sont identifiés comme électricité.
3 labels gas sont identifiés comme flood.
2 labels police sont identifiés comme fire.
2 labels fire brigad sont identifiés comme casualty.
2 labels flood sont identifiés comme casualty.
2 labels flood sont identifiés comme police.
2 label car est identifié comme accident.
1 label police est identifié comme casualty.
1 label stop  est identifié comme gas.

En conclusion, on retient de cette phase de test que les labels flood, électricité et casualty sont trop représentés alors que les labels flood, car et fire brigad ne sont pas
assez identifiés. On peut supposer que les lignes similaires dans les contours (verticales et horizontales disposées de manière proche) induisent les comparaisons en erreur,
surtout si les formes sont détériorées sur le scan. Concernant les affectations erronées plus anecdotiques, elles peuvent être simplement causées par des décalages ponctuels
dans les translations qui découpent imprécisément les icônes et conduisent à un échec de comparaison. Nous avons justement dressé un tableau des résultats par formulaire et
constaté que la plupart des échecs se concentrent sur un petit nombre de scans, suggérant donc que la qualité de leurs contours ou leur redressement faussent l’entièreté des
résultats générés.

Par ailleurs, dans cet ensemble, aucune taille n’était inscrite donc la configuration n’était pas idéale pour évaluer les performances. Cependant, nous avons pu tester sur notre
propre base d’apprentissage et les seuls échecs étaient dûs à un mauvais découpage de la zone de texte (biaisant les comparaisons). Ces erreurs provenaient donc des valeurs de
translations rendues caduques par un redressement des scans incorrect.

Ainsi, on a un rappel (nombre d’imagettes correctement extraites et liées aux bonnes informations / nombre total d’imagettes à obtenir) égal à la précision (nombre d’imagettes
correctement extraites et liées aux bonnes informations  / nombre d’imagettes obtenues), soit 0.53. On peut espérer améliorer cette valeur par la suite grâce à l’analyse des
cas d’échec.


Cas d’échec

Tout d’abord, le programme ne peut traiter les images si les croix pour le redressement ne sont pas reconnues (on suppose que le formulaire est trop détérioré ou ne contient
pas les informations attendues). Cependant, si elles sortent seulement légèrement du cadre du scan ou si elles ont été recouvertes (par exemple, si le scripteur a dessiné dessus),
le formulaire est laissé pour compte alors qu’il peut être correct. Le nombre de formulaires relevé est donc réduit : nous n’avons cependant aucun scan non traité suite à cette
étape avec cet ensemble de formulaires test. Néanmoins, sur le premier ensemble d’entraînement, nous avons constaté des échecs liés à ces problèmes.

Ensuite, certaines imagettes résultantes sont mal cadrées (notamment tronquées), dû à un mauvais redressement de l’image initiale. Les translations appliquées pour extraire
chaque symbole dessiné cumulent donc des erreurs de positionnement. Ce décalage initialement léger peut induire un manquement important en fin de boucle. Sur la base de test
fournie, cela concerne 19 sur 420 dessins.

Les icônes de la première colonne et les tailles renseignées peuvent subir les mêmes erreurs ce qui empêche l’algorithme de fournir une valeur de comparaison fiable entre les
modèles et les cibles extraites (alors mal découpées). De fait, l’ensemble de la ligne associée hérite d’un étiquetage erroné ou est abandonnée si la similarité est inférieure
au seuil fixé pour tous les modèles.

De plus, la qualité des scans est parfois contestable, provoquant des éclaircissements voire des trous dans les inscriptions et les contours. Si le traitement appliqué aux
images vise à rectifier autant que possible ces imperfections, il ne suffit pas à les résoudre entièrement. Les comparaisons sont d’autant plus biaisées que certaines icônes
ont des caractéristiques semblables, causant des confusions dans l’analyse des contours par le programme.

En fin de compte, les erreurs de translation représentent le risque que nous considérons comme le plus important puisqu’il engendre la majorité des échecs, à la fois de
reconnaissance des informations et de découpe des imagettes finales. Il peut donc condamner des lignes entières sur des formulaires initialement corrects. Cette erreur est
rectifiable en calculant des translations spécifiques à chaque formulaire, en améliorant la fonction de redressement des scans ou en identifiant les scans sujet aux erreurs
pour ne pas les traiter. Nous avons aussi constaté que le niveau de zoom pouvait varier d’un formulaire à un autre, engendrant lui aussi un décalage dans les découpes. Ce
point pourra être corrigé pour la prochaine échéance.



CONCLUSION :

Notre application permet donc à l’heure actuelle d’assurer les premières étapes du projet en extrayant les dessins des scripteurs et en en décrivant le contenu.
Des améliorations peuvent être apportées pour reconnaître les composantes avec plus de précision et couvrir les cas d’échec que nous avons identifiés.

Par ailleurs, les traitements appliqués présentent un vaste éventail d’ajustements possibles :
de nombreux paramètres et seuils interviennent et d’autres combinaisons de valeurs peuvent être testées.
Avec la nouvelle base de test, nous pouvons donc analyser les performances avec des paramètres différents et d’autres fonctions ajoutées, retirées ou dans un ordre différent
pour pouvoir choisir en fin de compte l’ensemble optimal.

Enfin, nous allons pouvoir commencer la seconde partie du projet à savoir l’extraction des fonctionnalités et la classification des dessins.
