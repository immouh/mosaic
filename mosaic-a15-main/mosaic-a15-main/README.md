The authors of project are:
-Yazid HANNACHI
-Olivier LATESTERE
-AIT-ISSAD MOHAMED 
The project contains:
 two main files (game_aux.h and game.h) that contain the main functions of the project. Additionally, each member has implemented testing functions for the project's functions. There's also a file called game_test.c, which appears to focus on launching a new game and correcting mistakes.



**SDL**
L'interface graphique de notre projet adopte un thème inspiré du monde fascinant du casino et du Far West. Cette interface se caractérise par sa simplicité et son style épuré, offrant une expérience utilisateur fluide et intuitive. À gauche de l'écran, vous trouverez des boutons permettant d'effectuer des actions telles que **Undo** et **Redo**, ainsi que d'autres fonctionnalités essentielles situées en bas de la fenêtre.
Pendant le jeu, si une case ne respecte pas une contrainte après un coup, elle est signalée en rouge. Si la contrainte est respectée, elle est affichée en vert. Cela aide les joueurs à suivre les règles et à prendre des décisions éclairées.
Par ailleurs, en ce qui concerne les fonctionnalités **Load** et **Save**, un fichier initialisé avec les paramètres de la partie par défaut est automatiquement créé. Lorsque l'utilisateur lance une nouvelle partie et appuie sur le bouton **Save**, les données de la partie en cours sont enregistrées dans ce fichier. Ainsi, grâce à la fonction **Load**, l'utilisateur peut restaurer la dernière partie enregistrée, lui offrant la possibilité de reprendre là où il s'était arrêté.

