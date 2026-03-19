# Contribute to Cairn users documentation : Guidelines

### Prérequis pour la documentation utilisateur :
- CMake >= 3.23
- MSVC C++ >=2019 (C++, CMake, Python, et avec l'extension pour Qt)
- Python avec les packages Sphinx, sphinx_rtd_theme
- MathJax >= 2.7.7 pour une utilisation offline (configuration faite pour une v2)


### Génération des fichiers RST des modèles à partir du code C++

Lancer le script `GenerateSphinxDoc.bat` de PerseeGUI. Le dossier Documentation doit se trouver dans PerseeGui/doc.

### Compilation

2 options :
- Avec cMake: ouvrir le projet doc_utilisateur avec Visual Studio -> générer tout le projet
Le résultat sera dans doc/Documentation/htmlSphinx
- Avec un .bat : TODO

Génération automatique de la doc après le build de PERSEE : TODO

### Balises:

```
.. important::
.. admonition::
```

### Structure de la doc:

Les titres sont écrits dans cet ordre ( à respecter pour ne pas casser l'arborescence !!)

```

#########
Titre 0
#########

Titre 1 
========

Titre 1.1
-----------

Titre 1.1.1
~~~~~~~~~~~~~~~~~~~~

Titre 1.1.1.1
^^^^^^^^^^^^^^^^^^^

Titre 1.1.1.1.1
*********************

```

### A propos de la génération via scripts python

- Les scripts de génération de la doc se trouvent dans PerseeGui/Scripts, fichiers `UserDocGen.py` et `CodeAnalyser.py`. On peut lancer `UserDocGen.py` pour réécrire les fichiers rst.
- Pour ne pas afficher une ligne (ou une méthode) : ecrire DO NOT SHOW en commentaire
- Pour une meilleure lisibilité du code, ne pas trop découper en fonctions si ce n'est pas nécessaire
- Pour les commentaires:
    - la balise `\\ ` ne sera pas gardée par le script
    - la balise `\** commentaire*\`, éventuellement sur plusieurs lignes, sera lue.


### Cacher/Montrer des parties 

Les balises suivantes permettent de conditionner l'affichage d'une partie:

- `cea_content`: contenu réservé aux utilisateurs CEA, par exemple: 
    - méthodes avancées, 
    - certains modèles dits "privés", 
    - manipulation des serveurs.

- `open_source`: ce qui est affiché par défaut quand cea_content est sur false, en remplacement éventuel.

```

.. ifconfig:: cea_content
    
    contenu affiché uniquement si "cea_content" est mis sur True dans conf.py

```

Attention !! Un fichier qui n'est pas dans l'arborescence est quand meme ajouté au dossier html. 
Il faut donc mettre toutes les informations à garder privées dans le dossier UserGuideSollus qui sera gardé sur un répertoire privé.

### Intégrer une image
```
.. image:: images/mutliconv_exemple.PNG

    :width: 100
    :alt: example multiconverter
    :align: center
```

### Ecrire des maths
En ligne: ``` :math:`hgfhfhgf` ```.
En paragraphe: `.. math::`.


 ## Inclure des rst

 ```
 .. include::file.rst
 ```

 ## Faire référence à une autre page dans la doc

Poser l'ancre: 

```
.. _cairn_map_file:
```

Y faire référence: 
```
:ref:`cairn_map_file`
```
**Ne pas oublier de retirer l'underscore !!**

## Citer des papiers

- Ajouter le papier dans Cairn.bib (on peut généralement télécharger directement au format `bib` depuis le site de l'éditeur):
```
@inproceedings{ruby_persee_2024,
	title = {PERSEE, a single tool for various optimizations of multi-carrier energy system sizing and operation},
	author = {Ruby, Alain and Gaoua, Yacine and Crevon, Stéphanie and Parmentier, Pimprenelle and Lavialle, Gilles},
	booktitle = {37th {International} {Conference} on {Efficiency}, {Cost}, {Optimization}, {Simulation} and {Environmental} {Impact} of {Energy} {Systems} ({ECOS} 2024)},
	year = {2024},
	url = {https://cea.hal.science/cea-04681216v1}
}
```
- Citer grâce au nom de référence: ``` :cite:`ruby_persee_2024` ```