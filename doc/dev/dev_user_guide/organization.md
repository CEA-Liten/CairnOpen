# Organization

## Rappels
Documentation de référence: https://git-scm.com/docs

Le repository ou dépôt est le lieu d'entreposage des sources 
La vue centralisée est hébergée sur le serveur TULEAP https://codev-tuleap.intra.cea.fr/plugins/git/persee/Persee.git

Sur ce repository il existe plusieurs branches:

- master est la branche officielle qui sert à toute diffusion industrielle vers l'extéreur de l'équipe de développement.

- Integration est la branche d'intégration vers laquelle converge tous les dévelopements avant fusion dans master quand la branche Integration est pleinement opérationnelle


Ces branches, sur un serveur distant, sont connues sous le nom remotes/origin/master et remotes/origin/Integration.

Pour travailler sur les sources, on travaille toujours sur une branche locale issue de la branche distante du dépôt. 
Par défaut leurs noms coïncident, localement on aura par exemple :

- une branche Integration image de remotes/origin/Integration - une branche master image de remotes/origin/master

Il y a donc toujours 4 étapes quand on utilise Git: 
- Etape 1 : extraire une copie du dépôt distant en local (on tire (pull) la branche Integration)

- Etape 2 : développer = modifier le dépôt local ET tester

- Etape 3 : enregistrer le dépôt local modifié (on engage les modifications faites "commit" dans la branche Integration)

- Etape 4 : publier cette image locale vers le dépôt distant ("push" envoi de Integration vers remotes/origin/Integration)

## Organisation et règles générales
L'équipe Cairn est organisée en 2 sous-groupes
- une équipe cœur de développeurs-intégrateurs Ali, Stéphanie et Pimprenelle, qui peuvent gérer les intégrations dans la branches, et suivent suffisamment le code pour être en mesure d’intégrer au bon moment (ou de ne pas le faire), et de recevoir des pull requests.

- une équipe étendue de développeurs qui ne pourront pas merger dans integration directement et devront passer par des pull requests. Les autres.

Chaque nouveau développement doit s’accompagner d’un test le validant, et d’une mise à jour de la documentation du code :
.pptx des modèles, vérification de la doc extraite, et mise à jour de Modele.json.
Les développeurs doivent toujours tester au moins localement par RunAllTNR.bat 
le résultat des tests après une modification de développement, de rebase ou de merge sur leur branche locale.

Il est possible à chaque développeur sur les branches autres qu'Integration de réaliser les tests en configuration "Jenkins"

Malgré cela, comme la branche integration est bien le lieu de tests des merges, elle est nécessairement moins stable que la master:
- il convient de se renseigner auprès de l’équipe cœur pour savoir si on peut partir de la branche integration pour développer une nouvelle branche. (je pense trop lourd de s’imposer systématiquement de ne démarrer que sur les tags Integration juste avant ou après un merge vers master)

Enfin, c’est master et elle seule qui doit être utilisée pour les sorties de résultats officiels vers l’extérieur, les livraisons aux partenaires …

Tableau de synthèse :

## Etape 1: Extraire une image locale d'une branche Git
Première extraction : git clone
Cela génère une copie locale image du dépôt distant, sur une branche master par défaut. Préciser la branche Integration.

Puis passer à l'étape 2.

Mettre à jour une copie locale sans modification : git revert + git pull
Lorsqu'on reprend une ancienne image locale, avant tout développement, il faut se remettre à jour sur la dernière image du dépôt distant:

git pull
cela fait un merge de toutes les modifications locales engagées ou non avec les mises à jour entrantes depuis le dépôt distant.

Si les sous-modules(Cairn et MIPModeler) sont modifiés, il faut les mettre à jour également:

git pull --recurse-submodules

Pour éviter des conflits, il vaut mieux faire un git revert avant qui remet le dépôt local dans les conditions du dernier commit.

Puis passer à l'étape 2.

## Etape 2: Réaliser et enregistrer son développement dans une image locale d'une branche Git
Sur la base de l'étape 1, on peut commencer à développer.

création d'une nouvelle branche (optionnel)
Création d'une nouvelle branche locale et d'une branche distante

Fichier:Exemple.jpg

Basculer sur cette branche : choisir switch / checkout

développement sur la branche locale
- On développe, on compile en local: buildAll.bat

- On teste l'image par run_Tnr.bat en local.

## Etape 3: Enregistrer son développement local
- On enregistre dans le dépôt local : git commit

En local, l'état modifié devient l'état courant. On est en décalé par rapport au dépôt distant qui continue à avancer.

On publiera vers le dépôt distant dans l'étape 4 par git push

## Etape 4: Publier son développement local vers le dépôt distant
Préliminaire : Remettre à jour son développement local : git fetch & rebase
Avant de publier vers le dépôt distant, il FAUT absolument:

interroger le dépôt distant pour savoir ce qui a bougé: git fetch
remettre son développement à niveau sur le dessus : git rebase (pull --rebase)
Cela évite des conflits inutiles détectés au moment de la publication

Puis repasser à l'étape 2: build, test :)

Publication push vers le dépôt distant[modifier]
Il suffit de faire git push.

Par défaut on push vers la branche distante de base utilisée pour baser son développement: remote/origine

Le fait de propager vers une autre branche (Integration par exemple) est une opération de Merge, voir étape 4.

Etape 4 : Merge : Fusionner son développement dans la branche Integration[modifier]
Mettre à jour les branches locales à partir du référentiel distant (branche intégration et branche de développement branch_dev) :

switch to Integration git checkout integration
git pull from remotes/origin/integration pour mettre à jour la branche locale (si c'est la première fois il y aura création d'une branche locale integration)

Fusionner en local :

Sur la branche integration on fait git merge branche_dev
La branche intégration local comporte maintenant les commits de la branche_dev.

Il est préférable de tester comme à l'étape 2 !
Il reste à faire remonter les modifications de la branche locale intégration sur la branche remote :

git push (to remotes/origin integration)
Et c'est fini. La branche remote intégration a incorporé tout le travail réalisé sur la branche de développement.

Autres commandes pratiques[modifier]
Connaître l'historique[modifier]
git log (show log)

Récupérer un commit fantôme[modifier]
Parfois dans les successions de switch entre branches, on peut avoir oublier de les publier vers le dépôt distant.

Si ensuite on réalise des merges, on risque de perdre la trace de ces commits locaux.

la commande git refLog permet de retrouver leur trace:

git reflog (show Reflog): liste les commits qui ont été réalisées, dans une branche ou hors branche.

Choisir/changer de branche[modifier]
switch/checkout branch

création d'une nouvelle branche locale : git branch -b : création d'une image locale d'un dépôt distant

Piquer juste un patch d’une branche sur l’autre[modifier]
Cherry pick

Merge pour réintegrer la branche Integration vers master[modifier]
Cette opération n'est autorisée qu'à l'intégrateur de l'équipe.

Mettre à jour les branches locales à partir du référentiel distant[modifier]
switch to Integration git pull from remotes/origin/integration (si c'est la première fois il y aura création d'une branche locale integration)

switch to master git pull from remotes/origin/master (si c'est la première fois il y aura création d'une branche locale master)

Fusionner en local[modifier]
git merge from Integration (car on est sur master)

Publier le merge[modifier]
git push (to remotes/origin master)