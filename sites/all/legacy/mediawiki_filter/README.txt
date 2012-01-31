Mediawiki filter
----------------

The Mediawiki filter is an input filter which uses the Mediawiki library
for formatting.


Installation
------------

See the 'lib/LIBRARY.txt' for a list of files needed from the Mediawiki
source.


Formatting
----------

You find information about the formatting supported on these pages:

http://en.wikipedia.org/wiki/Wikipedia:How_to_edit_a_page
http://en.wikipedia.org/wiki/Wikipedia:Extended_image_syntax
http://en.wikipedia.org/wiki/Help:Table

Also you can use templates as explained in
http://en.wikipedia.org/wiki/Help:Template





TODO:

mediawiki_filter

  - Admin settings page

  - Image handling
    - Check for resized images and use appropriate image
    - Check for config of image module, thumbnail should be 180?
  - Category to taxonomy
    - Parse category tags and create taxonomy terms
    - Node per term for category pages?
    - Use "Category" module?
    - Use "Book" module?
  - Language links
    - Parse language links and do something with it
  - Cache handling
    - Either clear formatting cache on every page creation (and changed titles)
    - Or keep information on referencing articles and clear cache of those
  - User page handling
    - Node per user?

  - Subsection editing
    - Use mediawiki parsers capabilities for subsection editing

  - Extensions
    - Look at a couple of extensions and check if library needs to be extended

mediawiki_import

  - Test for compatibility problems
  - List pages which contain problems, ability to "mark as resolved"
  - How to import images?
  - How to import user pages?

