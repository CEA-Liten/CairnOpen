import re

def parse_lp_file(file_path):
    """Lit un fichier .lp et retourne un dictionnaire avec les variables et les contraintes."""
    variables = set()
    constraints = {}
    compteur = 0
    obj = ""
    with open(file_path, 'r') as file:
        lines = file.readlines()
        
        is_constraint = False
        is_objective_function = False
        
        for line in lines:
            compteur+=1
            continue_line = False
            #objective_function = False
            if line.startswith("    "):
                continue_line=True
            line = line.strip()
            
            # Ignorer les lignes vides et les commentaires
            if not line or line.startswith("\\") or line.startswith("*"):
                print("skip:", line)
                continue
                
            if line.lower().startswith("obj"):
                is_objective_function = True
                continue
            if is_objective_function:
                obj = obj + line
            # Détecter la section des contraintes
            if line.lower().startswith("subject to") or line.lower().startswith("st"):
                is_constraint = True
                is_objective_function = False
                continue
            elif line.lower().startswith("bounds") or line.lower().startswith("end"):
                is_constraint = False
            
            if is_constraint:
                
                # Extraire la contrainte
                constraint_match = re.match(r"(\w+\d+):\s*(.+)", line)
                if constraint_match:
                    constraint_name = constraint_match.group(1)
                    constraint_expression = constraint_match.group(2)
                    constraints[constraint_name] =  constraint_expression
                elif continue_line:
                    constraint_expression = line
                    constraints[constraint_name] +=  constraint_expression
                if constraint_match or continue_line:
                    separateurs = r"[+,-,<=,=]" 
                    var_liste = re.split(separateurs,constraint_expression)
                    for i in var_liste:
                        try:
                            float(i)
                        except:
                            if len(i)>0:
                                variables.add(re.sub(r'^[\d\-.]+', '',(i.replace(' ',''))))
    print("compteur", compteur)
    return variables, constraints, obj

def compare_dictionaries(dict1, dict2):
    """Compare deux dictionnaires en retournant les éléments communs et différents."""
    keys_in_both = set(dict1.keys()).intersection(dict2.keys())
    keys_only_in_dict1 = set(dict1.keys()).difference(dict2.keys())
    keys_only_in_dict2 = set(dict2.keys()).difference(dict1.keys())
    
    diff_elements = {}
    for key in keys_in_both:
        if dict1[key] != dict2[key]:
            diff_elements[key] = (dict1[key], dict2[key])
    
    return keys_only_in_dict1, keys_only_in_dict2, diff_elements

def compare_sets(s1, s2):
    """Compare deux dictionnaires en retournant les éléments communs et différents."""
    return s1-s2, s2-s1, (s1-s2);add(s2-s1)

def test_comparaison(file1, file2):
    str_diff = ""
    identiques=True
    vars1, cons1, obj1 = parse_lp_file(file1)
    vars2, cons2, obj2 = parse_lp_file(file2)

    v1_only, v2_only, v_diff = compare_sets(vars1,vars2)
    c1_only, c2_only, c_diff = compare_dictionaries(cons1, cons2)

    if len(v_diff)>0:
        identiques = False
    if len(c_diff)>0:
        identiques = False
    if obj1!=obj2:
        identiques = False
    str_diff+=("====================================================\n")
    str_diff+=("comparaison des fichiers lp\n")
    str_diff+=("====================================================\n")
    if (identiques):
        str_diff+=("les fichiers sont identiques\n")
    else:
        str_diff+=("\n Variables uniquement dans le fichier 1:"+str(v1_only))
        str_diff+=("\n Variables uniquement dans le fichier 2:"+str(v2_only))
        str_diff+=("\n Différences de valeurs pour les variables communes:"+str(v_diff))

        str_diff+=("\nContraintes uniquement dans le fichier 1:"+str(c1_only))
        str_diff+=("\nContraintes uniquement dans le fichier 2:"+str(c2_only))
        str_diff+=("\nDifférences pour les contraintes communes:\n")
        for cle, valeur in c_diff.items():
            str_diff+=(f"{cle}: {valeur}"+"\n")
        if obj1!=obj2:
            str_diff+=("Objectifs differents:"+str(obj1)+str(obj2)+"\n")
        else:
            str_diff+=("Objectifs identiques")

    return(identiques,str_diff)

