# Principes d'architecture

Cairn est codé en C++ et basé sur Qt.
Cairn dispose aussi d'une API C++ et une API python réalisée avec PyBind11.

La classe CairnCore est le point d'entrée.

Elle contient un problème de type OptimProblem qui est composé de plusieurs composants de type MilpComponent.

Chaque composant a un modèle de type SubModel.

OptimProblem est lui même un MilpComponent et il contient un MIPModeler::MIPModel* mModel qui est le problème MILP qui va être envoyé à un solveur.

# Arborescence des modèles

BusSubModel
OperationSubModel
TechnicalSubModel

# Enchaînement lors d'un calcul

PerseeCore est construit sur un des principes de la norme FMI.

Il dispose donc de trois méthodes classiques : 
- doInit
- doStep
- doTerminate

Lorsqu'un calcul est lancé, le doInit puis le doStep sont appelés.

## doInit

Dans le doInit, le problème est lu progressivement.
Ce sont d'abord les paramètres de configuration qui sont lus.

## doStep

Dans le doStep, le problème MILP est construit grâce à toutes les méthodes buildProblem des modèles qui sont appelées successivement.
Ce sont d'abord les contraintes de composants qui sont construites.
Ensuite, ce sont les contraintes des bus qui sont construites.
Le problème est envoyé à un solveur.
La solution obtenue est analysée.
En fonction de la solution, les indicateurs sont calculé, les fichiers de résultats sont générés.

## doTerminate

Dans le doTerminate, le .json peut être écrit sous le nom _self.json si cette option est activée.
Puis les delete nécessaires sont faits.

# Focus sur le lien avec les données d'entrées

Cairn peut être utilisé en standalone.
Dans ce cas, les séries temporelles sont lues via un fichier .csv à fournir.
On passe alors dans la méthode importTS de PerseeCore qui possède des options de lecture (interpolation, average, persistence, périodique).

Cairn peut être utilisé dans une co-simulation.

Aujourd'hui, Cairn ne peut pas générer des FMUs.

Lorsqu'il est utilisé en co-simulation, certaines variables proviennent de l'extérieur et doivent avoir un nom spécifique.
Les séries temporelles sont directement récupérées de l'extérieur, la méthode importTS n'est pas appelée.
Les variables à fournir par l'extérieur s'appellent des variables MPC (Model Predictive Control).

Dans un modèle les données d'entrées/sorties sont définies par deux méthodes:
-  ```addIO(<name>, &<Expression>, <Units>) ;```

ajoute une donnée d'entrée/sortie définie par son nom, une expression du modèle est associée à cette donnée.

- addControlIO : ajoute une donnée d'entrée/sortie de type MPC ou RH (Rolling Horizon). Une ControlIO et une IO spéciale, elle contient en plus un historique des valeurs passées (et futures pour les RH).

Deux définitions possibles pour l'historique:
```
 addControlIO(<name>, &<Expression>, <Units>, &<Variable>, &<Default>) ;
 ```
Dans ce cas, l'historique est géré en interne par la ControlIO, seule la dernière valeur de l'historique est retournée dans la variable définie par ```<Variable>```
```
 addControlIO(<name>, &<Expression>, <Units>, &<Buffer>, &<Default>) ;
 ```
 Dans le cas, l'historique est géré en externe, c'est une adresse vers cet historique qui est passée en paramètre.
 Le stockage des données et la taille de l'historique sont gérés par la ControlIO, seul le calcul de la Variable résultante est laissée à la responsabilité du modèle.





# Focus sur le développement d'un modèle

Un modèle implémente nécessairement les méthodes suivantes :
- declareModelConfigurationParameters()
- declareModelParameters()
- declareModelInterface()
- declareModelIndicators()
- buildModel()

Un modèle a des ports par défaut qu'il faut indiquer dans le modèle.

Dans la méthode buildModel() qui sert à ajouter les contraintes, chaque variable doit être créée puis ajoutée.
Une variable peut être une MIPVariable0D, 1D, 2D.
L'ajout de la variable au modèle se fait grâce à la méthode addVariable.