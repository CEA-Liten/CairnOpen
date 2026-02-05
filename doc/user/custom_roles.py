from docutils import nodes
from docutils.parsers.rst import roles

def raw_cairn_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    """
    Rôle personnalisé pour changer la police du texte
    """
    # Créer un nœud de texte avec une classe CSS personnalisée
    node = nodes.inline(text, classes=['rc'])

    return [node], []

def setup(app):
    # Enregistrer le rôle
    app.add_role('rc', raw_cairn_role)
    return {
        'version': '1.0',
        'parallel_read_safe': True,
    }