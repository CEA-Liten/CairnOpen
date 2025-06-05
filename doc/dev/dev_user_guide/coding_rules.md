# Coding rules

## Rules to write a submodel

- Supprimer les membres qui ne sont pas utilisés
- Vérifier dans la déclaration des paramètres s'il n'y a pas de doublon (.h)
- Vérifier que les initialisation de variables ne sont faites qu'une fois définir les contraintes du modèle, regroupées par blocs logiques, ne pas tout combiner dans une même boucle for mais séparer par blocs logiques, mettre un commentaire avant le bloc logique pour décrire en anglais la/les contraintes du bloc.
utiliser les "+=" ou "-=" uniquement pour modifier une expression déjà remplie par ailleurs
- Enrichir les consistency check si besoin pour vérifier la cohérence des paramétrages
- Vérifier que mAllocate = false est bien en fin de méthode.
- Ajouter de la documentation en tête du .h pour bien décrire les options et leurs fonctionnements (pour la doc et l'IHM) (en anglais)
- Pour chaque addParameter, placer la ligne de doc doc dans la fonction et pas en commentaire. Ne pas mettre de virgule.
- Vérifier qu'il est possible de comprendre ce que font les options sans lire le code (uniquement grâce à la doc)
- Option & paramètre : mettre dans le addParameter la variable de condition au lieu de mettre un if (condition)
- Vérifier que dans la déclaration des paramètres de config il n'y a que des booléens
- Utiliser les expressions dans les contraintes à la place des variables
- Mettre les valeurs par défaut dans le .cpp dans le constructeur
- Nommer les expressions en mExpX, les variables en mVarX, avec X le nom en anglais
- Nommer les variables historiques en mHistX et les dataseries en mXProfile.
- Dans le .h, déclarer d'abord toutes les variables puis toutes les expressions dans un ordre logique, puis les variables historiques et retours d'état, puis les inputs par type (bool puis les autres), puis les dataseries.
- Nommer toutes les variables dans mModel->add(...) avec CName et le même nom que la variable (si mVarX, alors appeler la variable X). idem pour les contraintes
