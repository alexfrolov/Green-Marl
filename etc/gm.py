# -*- coding: utf-8 -*-
"""
    pygments.lexers.gm
    ~~~~~~~~~~~~~~~~~~~

    Lexers for the GreenMarl programming language.

    :copyright: Copyright 2006-2015 by the Pygments team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import re

from pygments.lexer import RegexLexer
from pygments.token import Text, Comment, Operator, Keyword, Name, String, \
    Number, Punctuation, Error

__all__ = ['GreenMarlLexer']

class GreenMarlLexer(RegexLexer):
    """
    For the GreenMarl language.

    .. versionadded:: 0.1
    """

    name = 'GreenMarl'
    aliases = ['gm', 'xten']
    filenames = ['*.gm']
    mimetypes = ['text/x-gm']

    keywords = (
        'Procedure', 'Return',
        'Foreach',
        'Graph', 'Nodes', 'InNbrs',
				'Count', 'Avg'
    )

    types = (
				'Boolean', 'Int', 'Float', 'Double',
    )

    values = (
        'false', 'NIL', 'this', 'true'
    )

    modifiers = (
				'N_P','E_P'
    )

    tokens = {
        'root': [
            (r'[^\S\n]+', Text),
            (r'//.*?\n', Comment.Single),
            (r'/\*(.|\n)*?\*/', Comment.Multiline),
            (r'\b(%s)\b' % '|'.join(keywords), Keyword),
            (r'\b(%s)\b' % '|'.join(types), Keyword.Type),
            (r'\b(%s)\b' % '|'.join(values), Keyword.Constant),
            (r'\b(%s)\b' % '|'.join(modifiers), Keyword.Declaration),
            (r'"(\\\\|\\"|[^"])*"', String),
            (r"'\\.'|'[^\\]'|'\\u[0-9a-fA-F]{4}'", String.Char),
            (r'.', Text)
        ],
    }
