
TML - Tiny Markup Language
==========================

Specification:

TML is a human-readable smart data-serialization language. Something like YAML, JSON, XML.

The content of a TML file consists of one or more name-value pairs. The following is an example with different name-value pairs.

	someText = hello
	a-number = 13
	perentage = 0.34
	array-with-names = Alice Bob Carol Eve Mallory
	aObjectWithTml
		name = Max
		weight = 71.7
		body-height = 123
	use-mipmap = false
	active = true
	asdf = null
	# this is a comment

	text = "text with spaces must be between apostrophes"

A indent must be one or more tabs or one or more spaces.
The first indent defines the characters for a indent (must be one or more tabs or one or more spaces). Mixing is not allowed!

TML supports following types:

 * **Null**:             `null`
 * **Boolean**:          Can be `true` or `false`
 * **Integer**:          Sequence of digits. Optional starting with `+` or `-`.
 * **Floating point**:   Sequence of digits with a dot `.`. Optional starting with `+` or `-`.
 * **Text**:             A Text is everthing which is not a Null, a Boolean, a Integer and not a Floating point.
                         e.g. `ThisIsAText123`. A text with spaces is not classified a simple text.
                         If a text with spaces is necessary then apostrophes must be used.
                         e.g. `"This is a example with spaces"`.
 * **Array**:            The elements of an array are separated with spaces.
 * **Object (Section)**: An object has an object name at the first line without a value (no = ... at the first line).
                         The members of of the object are the following lines. The members must be indent.
			 An object can also be seen as a section (like from an INI file).

Objects can be nested like the following example:

	aObject
		name = Max
		weight = 71.7
		body-height = 123

		subObj
			idName = TEX123
			links = 3
			date = null
			counter = 3
			numbers = 4 2 8 0 2 4

		subObj
			idName = OBJ321
			links = 6
			numbers = 4.3 2.1 8 0 2 4

			otherObj
				idName = 321
				links = 6
				numbers = 4 2 8 0 2 4

		subObj
			idName = 321-A
			links = 6
			numbers = 4 2 8 0 2 4


