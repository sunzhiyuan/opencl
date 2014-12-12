//--------------------------------------------------------------------------------------
// File: TinyXML.cpp
// Desc: TinyXML xml parser stripped down to read-only functionality
//--------------------------------------------------------------------------------------
/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2006 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#include <ctype.h>
#include <stddef.h>
#include "tinyxml.h"


bool TiXmlBase::condenseWhiteSpace = true;

TiXmlNode::TiXmlNode( NodeType _type ) : TiXmlBase()
{
    parent = 0;
    type = _type;
    firstChild = 0;
    lastChild = 0;
    prev = 0;
    next = 0;
}


TiXmlNode::~TiXmlNode()
{
    TiXmlNode* node = firstChild;
    TiXmlNode* temp = 0;

    while ( node )
    {
        temp = node;
        node = node->next;
        delete temp;
    }	
}


void TiXmlNode::Clear()
{
    TiXmlNode* node = firstChild;
    TiXmlNode* temp = 0;

    while ( node )
    {
        temp = node;
        node = node->next;
        delete temp;
    }	

    firstChild = 0;
    lastChild = 0;
}


TiXmlNode* TiXmlNode::LinkEndChild( TiXmlNode* node )
{
    assert( node->parent == 0 || node->parent == this );
    assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

    if ( node->Type() == TiXmlNode::DOCUMENT )
    {
        delete node;
        if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return 0;
    }

    node->parent = this;

    node->prev = lastChild;
    node->next = 0;

    if ( lastChild )
        lastChild->next = node;
    else
        firstChild = node;			// it was an empty list.

    lastChild = node;
    return node;
}


const TiXmlNode* TiXmlNode::FirstChild( const char * _value ) const
{
    const TiXmlNode* node;
    for ( node = firstChild; node; node = node->next )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const TiXmlNode* TiXmlNode::LastChild( const char * _value ) const
{
    const TiXmlNode* node;
    for ( node = lastChild; node; node = node->prev )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const TiXmlNode* TiXmlNode::NextSibling( const char * _value ) const 
{
    const TiXmlNode* node;
    for ( node = next; node; node = node->next )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const TiXmlNode* TiXmlNode::PreviousSibling( const char * _value ) const
{
    const TiXmlNode* node;
    for ( node = prev; node; node = node->prev )
    {
        if ( strcmp( node->Value(), _value ) == 0 )
            return node;
    }
    return 0;
}


const TiXmlElement* TiXmlNode::FirstChildElement() const
{
    const TiXmlNode* node;

    for (	node = FirstChild();
            node;
            node = node->NextSibling() )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const TiXmlElement* TiXmlNode::FirstChildElement( const char * _value ) const
{
    const TiXmlNode* node;

    for (	node = FirstChild( _value );
            node;
            node = node->NextSibling( _value ) )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const TiXmlElement* TiXmlNode::NextSiblingElement() const
{
    const TiXmlNode* node;

    for (	node = NextSibling();
            node;
            node = node->NextSibling() )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const TiXmlElement* TiXmlNode::NextSiblingElement( const char * _value ) const
{
    const TiXmlNode* node;

    for (	node = NextSibling( _value );
            node;
            node = node->NextSibling( _value ) )
    {
        if ( node->ToElement() )
            return node->ToElement();
    }
    return 0;
}


const TiXmlDocument* TiXmlNode::GetDocument() const
{
    const TiXmlNode* node;

    for( node = this; node; node = node->parent )
    {
        if ( node->ToDocument() )
            return node->ToDocument();
    }
    return 0;
}


TiXmlElement::TiXmlElement (const char * _value)
    : TiXmlNode( TiXmlNode::ELEMENT )
{
    firstChild = lastChild = 0;
    value = _value;
}


TiXmlElement::~TiXmlElement()
{
    ClearThis();
}


void TiXmlElement::ClearThis()
{
    Clear();
    while( attributeSet.First() )
    {
        TiXmlAttribute* node = attributeSet.First();
        attributeSet.Remove( node );
        delete node;
    }
}


const char* TiXmlElement::Attribute( const char* name ) const
{
    const TiXmlAttribute* node = attributeSet.Find( name );
    if ( node )
        return node->Value();
    return 0;
}


int TiXmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
    const TiXmlAttribute* node = attributeSet.Find( name );
    if ( !node )
        return TIXML_NO_ATTRIBUTE;
    return node->QueryIntValue( ival );
}


const char* TiXmlElement::GetText() const
{
    const TiXmlNode* child = this->FirstChild();
    if ( child ) {
        const TiXmlText* childText = child->ToText();
        if ( childText ) {
            return childText->Value();
        }
    }
    return 0;
}


TiXmlDocument::TiXmlDocument() : TiXmlNode( TiXmlNode::DOCUMENT )
{
    tabsize = 4;
    ClearError();
}

bool TiXmlDocument::LoadFile( const char* _filename, TiXmlEncoding encoding )
{
    // There was a really terrifying little bug here. The code:
    //		value = filename
    // in the STL case, cause the assignment method of the std::string to
    // be called. What is strange, is that the std::string had the same
    // address as it's c_str() method, and so bad things happen. Looks
    // like a bug in the Microsoft STL implementation.
    // Add an extra string to avoid the crash.
    TIXML_STRING filename( _filename );
    value = filename;

    // reading in binary mode so that tinyxml can normalize the EOL
    FILE* file = fopen( value.c_str (), "rb" );	

    if ( file )
    {
        bool result = LoadFile( file, encoding );
        fclose( file );
        return result;
    }
    else
    {
        SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
        return false;
    }
}

bool TiXmlDocument::LoadFile( FILE* file, TiXmlEncoding encoding )
{
    if ( !file ) 
    {
        SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
        return false;
    }

    // Delete the existing data:
    Clear();
    location.Clear();

    // Get the file size, so we can pre-allocate the string. HUGE speed impact.
    long length = 0;
    fseek( file, 0, SEEK_END );
    length = ftell( file );
    fseek( file, 0, SEEK_SET );

    // Strange case, but good to handle up front.
    if ( length == 0 )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return false;
    }

    // If we have a file, assume it is all one big XML file, and read it in.
    // The document parser may decide the document ends sooner than the entire file, however.
    TIXML_STRING data;
    data.reserve( length );

    // Subtle bug here. TinyXml did use fgets. But from the XML spec:
    // 2.11 End-of-Line Handling
    // <snip>
    // <quote>
    // ...the XML processor MUST behave as if it normalized all line breaks in external 
    // parsed entities (including the document entity) on input, before parsing, by translating 
    // both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
    // a single #xA character.
    // </quote>
    //
    // It is not clear fgets does that, and certainly isn't clear it works cross platform. 
    // Generally, you expect fgets to translate from the convention of the OS to the c/unix
    // convention, and not work generally.

    /*
    while( fgets( buf, sizeof(buf), file ) )
    {
        data += buf;
    }
    */

    char* buf = new char[ length+1 ];
    buf[0] = 0;

    if ( fread( buf, length, 1, file ) != 1 ) {
        delete [] buf;
        SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
        return false;
    }

    const char* lastPos = buf;
    const char* p = buf;

    buf[length] = 0;
    while( *p ) {
        assert( p < (buf+length) );
        if ( *p == 0xa ) {
            // Newline character. No special rules for this. Append all the characters
            // since the last string, and include the newline.
            data.append( lastPos, (p-lastPos+1) );	// append, include the newline
            ++p;									// move past the newline
            lastPos = p;							// and point to the new buffer (may be 0)
            assert( p <= (buf+length) );
        }
        else if ( *p == 0xd ) {
            // Carriage return. Append what we have so far, then
            // handle moving forward in the buffer.
            if ( (p-lastPos) > 0 ) {
                data.append( lastPos, p-lastPos );	// do not add the CR
            }
            data += (char)0xa;						// a proper newline

            if ( *(p+1) == 0xa ) {
                // Carriage return - new line sequence
                p += 2;
                lastPos = p;
                assert( p <= (buf+length) );
            }
            else {
                // it was followed by something else...that is presumably characters again.
                ++p;
                lastPos = p;
                assert( p <= (buf+length) );
            }
        }
        else {
            ++p;
        }
    }
    // Handle any left over characters.
    if ( p-lastPos ) {
        data.append( lastPos, p-lastPos );
    }		
    delete [] buf;
    buf = 0;

    Parse( data.c_str(), 0, encoding );

    if (  Error() )
        return false;
    else
        return true;
}


int TiXmlAttribute::QueryIntValue( int* ival ) const
{
    if ( sscanf( value.c_str(), "%d", ival ) == 1 )
        return TIXML_SUCCESS;
    return TIXML_WRONG_TYPE;
}

TiXmlDeclaration::TiXmlDeclaration( const char * _version,
                                    const char * _encoding,
                                    const char * _standalone )
    : TiXmlNode( TiXmlNode::DECLARATION )
{
    version = _version;
    encoding = _encoding;
    standalone = _standalone;
}


TiXmlAttributeSet::TiXmlAttributeSet()
{
    sentinel.next = &sentinel;
    sentinel.prev = &sentinel;
}


TiXmlAttributeSet::~TiXmlAttributeSet()
{
    assert( sentinel.next == &sentinel );
    assert( sentinel.prev == &sentinel );
}


void TiXmlAttributeSet::Add( TiXmlAttribute* addMe )
{
    addMe->next = &sentinel;
    addMe->prev = sentinel.prev;

    sentinel.prev->next = addMe;
    sentinel.prev      = addMe;
}

void TiXmlAttributeSet::Remove( TiXmlAttribute* removeMe )
{
    TiXmlAttribute* node;

    for( node = sentinel.next; node != &sentinel; node = node->next )
    {
        if ( node == removeMe )
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            node->next = 0;
            node->prev = 0;
            return;
        }
    }
    assert( 0 );		// we tried to remove a non-linked attribute.
}


const TiXmlAttribute* TiXmlAttributeSet::Find( const char* name ) const
{
    for( const TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
    {
        if ( strcmp( node->name.c_str(), name ) == 0 )
            return node;
    }
    return 0;
}


const char* TiXmlBase::errorString[ TIXML_ERROR_STRING_COUNT ] =
{
    "No error",
    "Error",
    "Failed to open file",
    "Memory allocation failed.",
    "Error parsing Element.",
    "Failed to read Element name",
    "Error reading Element value.",
    "Error reading Attributes.",
    "Error: empty tag.",
    "Error reading end tag.",
    "Error parsing Unknown.",
    "Error parsing Comment.",
    "Error parsing Declaration.",
    "Error document empty.",
    "Error null (0) or unexpected EOF found in input stream.",
    "Error parsing CDATA.",
    "Error when TiXmlDocument added to document, because TiXmlDocument can only be at the root.",
};


//#define DEBUG_PARSER
#if defined( DEBUG_PARSER )
#	if defined( DEBUG ) && defined( _MSC_VER )
#		include <windows.h>
#		define TIXML_LOG OutputDebugString
#	else
#		define TIXML_LOG printf
#	endif
#endif

// This
// is less flexible than it appears. Changing the entries
// or order will break putstring.	
TiXmlBase::Entity TiXmlBase::entity[ NUM_ENTITY ] = 
{
    { "&amp;",  5, '&' },
    { "&lt;",   4, '<' },
    { "&gt;",   4, '>' },
    { "&quot;", 6, '\"' },
    { "&apos;", 6, '\'' }
};

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
// Including the basic of this table, which determines the #bytes in the
// sequence from the lead byte. 1 placed for invalid sequences --
// although the result will be junk, pass it through as much as possible.
// Beware of the non-characters in UTF-8:	
//				ef bb bf (Microsoft "lead bytes")
//				ef bf be
//				ef bf bf 

const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

const int TiXmlBase::utf8ByteTable[256] = 
{
    //	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x00
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x10
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x20
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x30
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x40
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x50
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x60
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x70	End of ASCII range
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x80 0x80 to 0xc1 invalid
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x90 
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xa0 
        1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xb0 
        1,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xc0 0xc2 to 0xdf 2 byte
        2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xd0
        3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	// 0xe0 0xe0 to 0xef 3 byte
        4,	4,	4,	4,	4,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1	// 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};


void TiXmlBase::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (input < 0x80) 
        *length = 1;
    else if ( input < 0x800 )
        *length = 2;
    else if ( input < 0x10000 )
        *length = 3;
    else if ( input < 0x200000 )
        *length = 4;
    else
        { *length = 0; return; }	// This code won't covert this correctly anyway.

    output += *length;

    // Scary scary fall throughs.
    switch (*length) 
    {
        case 4:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 3:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 2:
            --output; 
            *output = (char)((input | BYTE_MARK) & BYTE_MASK); 
            input >>= 6;
        case 1:
            --output; 
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
    }
}


/*static*/ int TiXmlBase::IsAlpha( unsigned char anyByte, TiXmlEncoding /*encoding*/ )
{
    // This will only work for low-ascii, everything else is assumed to be a valid
    // letter. I'm not sure this is the best approach, but it is quite tricky trying
    // to figure out alhabetical vs. not across encoding. So take a very 
    // conservative approach.

//	if ( encoding == TIXML_ENCODING_UTF8 )
//	{
        if ( anyByte < 127 )
            return isalpha( anyByte );
        else
            return 1;	// What else to do? The unicode set is huge...get the english ones right.
//	}
//	else
//	{
//		return isalpha( anyByte );
//	}
}


/*static*/ int TiXmlBase::IsAlphaNum( unsigned char anyByte, TiXmlEncoding /*encoding*/ )
{
    // This will only work for low-ascii, everything else is assumed to be a valid
    // letter. I'm not sure this is the best approach, but it is quite tricky trying
    // to figure out alhabetical vs. not across encoding. So take a very 
    // conservative approach.

//	if ( encoding == TIXML_ENCODING_UTF8 )
//	{
        if ( anyByte < 127 )
            return isalnum( anyByte );
        else
            return 1;	// What else to do? The unicode set is huge...get the english ones right.
//	}
//	else
//	{
//		return isalnum( anyByte );
//	}
}


class TiXmlParsingData
{
    friend class TiXmlDocument;
  public:
    void Stamp( const char* now, TiXmlEncoding encoding );

    const TiXmlCursor& Cursor()	{ return cursor; }

  private:
    // Only used by the document!
    TiXmlParsingData( const char* start, int _tabsize, int row, int col )
    {
        assert( start );
        stamp = start;
        tabsize = _tabsize;
        cursor.row = row;
        cursor.col = col;
    }

    TiXmlCursor		cursor;
    const char*		stamp;
    int				tabsize;
};


void TiXmlParsingData::Stamp( const char* now, TiXmlEncoding encoding )
{
    assert( now );

    // Do nothing if the tabsize is 0.
    if ( tabsize < 1 )
    {
        return;
    }

    // Get the current row, column.
    int row = cursor.row;
    int col = cursor.col;
    const char* p = stamp;
    assert( p );

    while ( p < now )
    {
        // Treat p as unsigned, so we have a happy compiler.
        const unsigned char* pU = (const unsigned char*)p;

        // Code contributed by Fletcher Dunn: (modified by lee)
        switch (*pU) {
            case 0:
                // We *should* never get here, but in case we do, don't
                // advance past the terminating null character, ever
                return;

            case '\r':
                // bump down to the next line
                ++row;
                col = 0;				
                // Eat the character
                ++p;

                // Check for \r\n sequence, and treat this as a single character
                if (*p == '\n') {
                    ++p;
                }
                break;

            case '\n':
                // bump down to the next line
                ++row;
                col = 0;

                // Eat the character
                ++p;

                // Check for \n\r sequence, and treat this as a single
                // character.  (Yes, this bizarre thing does occur still
                // on some arcane platforms...)
                if (*p == '\r') {
                    ++p;
                }
                break;

            case '\t':
                // Eat the character
                ++p;

                // Skip to next tab stop
                col = (col / tabsize + 1) * tabsize;
                break;

            case TIXML_UTF_LEAD_0:
                if ( encoding == TIXML_ENCODING_UTF8 )
                {
                    if ( *(p+1) && *(p+2) )
                    {
                        // In these cases, don't advance the column. These are
                        // 0-width spaces.
                        if ( *(pU+1)==TIXML_UTF_LEAD_1 && *(pU+2)==TIXML_UTF_LEAD_2 )
                            p += 3;	
                        else if ( *(pU+1)==0xbfU && *(pU+2)==0xbeU )
                            p += 3;	
                        else if ( *(pU+1)==0xbfU && *(pU+2)==0xbfU )
                            p += 3;	
                        else
                            { p +=3; ++col; }	// A normal character.
                    }
                }
                else
                {
                    ++p;
                    ++col;
                }
                break;

            default:
                if ( encoding == TIXML_ENCODING_UTF8 )
                {
                    // Eat the 1 to 4 byte utf8 character.
                    int step = TiXmlBase::utf8ByteTable[*((const unsigned char*)p)];
                    if ( step == 0 )
                        step = 1;		// Error case from bad encoding, but handle gracefully.
                    p += step;

                    // Just advance one column, of course.
                    ++col;
                }
                else
                {
                    ++p;
                    ++col;
                }
                break;
        }
    }
    cursor.row = row;
    cursor.col = col;
    assert( cursor.row >= -1 );
    assert( cursor.col >= -1 );
    stamp = p;
    assert( stamp );
}


const char* TiXmlBase::SkipWhiteSpace( const char* p, TiXmlEncoding encoding )
{
    if ( !p || !*p )
    {
        return 0;
    }
    if ( encoding == TIXML_ENCODING_UTF8 )
    {
        while ( *p )
        {
            const unsigned char* pU = (const unsigned char*)p;
            
            // Skip the stupid Microsoft UTF-8 Byte order marks
            if (	*(pU+0)==TIXML_UTF_LEAD_0
                 && *(pU+1)==TIXML_UTF_LEAD_1 
                 && *(pU+2)==TIXML_UTF_LEAD_2 )
            {
                p += 3;
                continue;
            }
            else if(*(pU+0)==TIXML_UTF_LEAD_0
                 && *(pU+1)==0xbfU
                 && *(pU+2)==0xbeU )
            {
                p += 3;
                continue;
            }
            else if(*(pU+0)==TIXML_UTF_LEAD_0
                 && *(pU+1)==0xbfU
                 && *(pU+2)==0xbfU )
            {
                p += 3;
                continue;
            }

            if ( IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' )		// Still using old rules for white space.
                ++p;
            else
                break;
        }
    }
    else
    {
        while ( *p && IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' )
            ++p;
    }

    return p;
}

// One of TinyXML's more performance demanding functions. Try to keep the memory overhead down. The
// "assign" optimization removes over 10% of the execution time.
//
const char* TiXmlBase::ReadName( const char* p, TIXML_STRING * name, TiXmlEncoding encoding )
{
    // Oddly, not supported on some comilers,
    //name->clear();
    // So use this:
    *name = "";
    assert( p );

    // Names start with letters or underscores.
    // Of course, in unicode, tinyxml has no idea what a letter *is*. The
    // algorithm is generous.
    //
    // After that, they can be letters, underscores, numbers,
    // hyphens, or colons. (Colons are valid ony for namespaces,
    // but tinyxml can't tell namespaces from names.)
    if (    p && *p 
         && ( IsAlpha( (unsigned char) *p, encoding ) || *p == '_' ) )
    {
        const char* start = p;
        while(		p && *p
                &&	(		IsAlphaNum( (unsigned char ) *p, encoding ) 
                         || *p == '_'
                         || *p == '-'
                         || *p == '.'
                         || *p == ':' ) )
        {
            //(*name) += *p; // expensive
            ++p;
        }
        if ( p-start > 0 ) {
            name->assign( start, p-start );
        }
        return p;
    }
    return 0;
}

const char* TiXmlBase::GetEntity( const char* p, char* value, int* length, TiXmlEncoding encoding )
{
    // Presume an entity, and pull it out.
    TIXML_STRING ent;
    int i;
    *length = 0;

    if ( *(p+1) && *(p+1) == '#' && *(p+2) )
    {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if ( *(p+2) == 'x' )
        {
            // Hexadecimal.
            if ( !*(p+3) ) return 0;

            const char* q = p+3;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != 'x' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else if ( *q >= 'a' && *q <= 'f' )
                    ucs += mult * (*q - 'a' + 10);
                else if ( *q >= 'A' && *q <= 'F' )
                    ucs += mult * (*q - 'A' + 10 );
                else 
                    return 0;
                mult *= 16;
                --q;
            }
        }
        else
        {
            // Decimal.
            if ( !*(p+2) ) return 0;

            const char* q = p+2;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != '#' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else 
                    return 0;
                mult *= 10;
                --q;
            }
        }
        if ( encoding == TIXML_ENCODING_UTF8 )
        {
            // convert the UCS to UTF-8
            ConvertUTF32ToUTF8( ucs, value, length );
        }
        else
        {
            *value = (char)ucs;
            *length = 1;
        }
        return p + delta + 1;
    }

    // Now try to match it.
    for( i=0; i<NUM_ENTITY; ++i )
    {
        if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
        {
            assert( strlen( entity[i].str ) == entity[i].strLength );
            *value = entity[i].chr;
            *length = 1;
            return ( p + entity[i].strLength );
        }
    }

    // So it wasn't an entity, its unrecognized, or something like that.
    *value = *p;	// Don't put back the last one, since we return it!
    //*length = 1;	// Leave unrecognized entities - this doesn't really work.
                    // Just writes strange XML.
    return p+1;
}


bool TiXmlBase::StringEqual( const char* p,
                             const char* tag,
                             bool ignoreCase,
                             TiXmlEncoding encoding )
{
    assert( p );
    assert( tag );
    if ( !p || !*p )
    {
        assert( 0 );
        return false;
    }

    const char* q = p;

    if ( ignoreCase )
    {
        while ( *q && *tag && ToLower( *q, encoding ) == ToLower( *tag, encoding ) )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )
            return true;
    }
    else
    {
        while ( *q && *tag && *q == *tag )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )		// Have we found the end of the tag, and everything equal?
            return true;
    }
    return false;
}

const char* TiXmlBase::ReadText(	const char* p, 
                                    TIXML_STRING * text, 
                                    bool trimWhiteSpace, 
                                    const char* endTag, 
                                    bool caseInsensitive,
                                    TiXmlEncoding encoding )
{
    *text = "";
    if (    !trimWhiteSpace			// certain tags always keep whitespace
         || !condenseWhiteSpace )	// if true, whitespace is always kept
    {
        // Keep all the white space.
        while (	   p && *p
                && !StringEqual( p, endTag, caseInsensitive, encoding )
              )
        {
            int len;
            char cArr[4] = { 0, 0, 0, 0 };
            p = GetChar( p, cArr, &len, encoding );
            text->append( cArr, len );
        }
    }
    else
    {
        bool whitespace = false;

        // Remove leading white space:
        p = SkipWhiteSpace( p, encoding );
        while (	   p && *p
                && !StringEqual( p, endTag, caseInsensitive, encoding ) )
        {
            if ( *p == '\r' || *p == '\n' )
            {
                whitespace = true;
                ++p;
            }
            else if ( IsWhiteSpace( *p ) )
            {
                whitespace = true;
                ++p;
            }
            else
            {
                // If we've found whitespace, add it before the
                // new character. Any whitespace just becomes a space.
                if ( whitespace )
                {
                    (*text) += ' ';
                    whitespace = false;
                }
                int len;
                char cArr[4] = { 0, 0, 0, 0 };
                p = GetChar( p, cArr, &len, encoding );
                if ( len == 1 )
                    (*text) += cArr[0];	// more efficient
                else
                    text->append( cArr, len );
            }
        }
    }
    if ( p ) 
        p += strlen( endTag );
    return p;
}

const char* TiXmlDocument::Parse( const char* p, TiXmlParsingData* prevData, TiXmlEncoding encoding )
{
    ClearError();

    // Parse away, at the document level. Since a document
    // contains nothing but other tags, most of what happens
    // here is skipping white space.
    if ( !p || !*p )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return 0;
    }

    // Note that, for a document, this needs to come
    // before the while space skip, so that parsing
    // starts from the pointer we are given.
    location.Clear();
    if ( prevData )
    {
        location.row = prevData->cursor.row;
        location.col = prevData->cursor.col;
    }
    else
    {
        location.row = 0;
        location.col = 0;
    }
    TiXmlParsingData data( p, TabSize(), location.row, location.col );
    location = data.Cursor();

    if ( encoding == TIXML_ENCODING_UNKNOWN )
    {
        // Check for the Microsoft UTF-8 lead bytes.
        const unsigned char* pU = (const unsigned char*)p;
        if (	*(pU+0) && *(pU+0) == TIXML_UTF_LEAD_0
             && *(pU+1) && *(pU+1) == TIXML_UTF_LEAD_1
             && *(pU+2) && *(pU+2) == TIXML_UTF_LEAD_2 )
        {
            encoding = TIXML_ENCODING_UTF8;
        }
    }

    p = SkipWhiteSpace( p, encoding );
    if ( !p )
    {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
        return 0;
    }

    while ( p && *p )
    {
        TiXmlNode* node = Identify( p, encoding );
        if ( node )
        {
            p = node->Parse( p, &data, encoding );
            LinkEndChild( node );
        }
        else
        {
            break;
        }

        // Did we get encoding info?
        if (    encoding == TIXML_ENCODING_UNKNOWN
             && node->ToDeclaration() )
        {
            TiXmlDeclaration* dec = node->ToDeclaration();
            const char* enc = dec->Encoding();
            assert( enc );

            if ( *enc == 0 )
                encoding = TIXML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF-8", true, TIXML_ENCODING_UNKNOWN ) )
                encoding = TIXML_ENCODING_UTF8;
            else if ( StringEqual( enc, "UTF8", true, TIXML_ENCODING_UNKNOWN ) )
                encoding = TIXML_ENCODING_UTF8;	// incorrect, but be nice
            else 
                encoding = TIXML_ENCODING_LEGACY;
        }

        p = SkipWhiteSpace( p, encoding );
    }

    // Was this empty?
    if ( !firstChild ) {
        SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, encoding );
        return 0;
    }

    // All is well.
    return p;
}

void TiXmlDocument::SetError( int err, const char* pError, TiXmlParsingData* data, TiXmlEncoding encoding )
{	
    // The first error in a chain is more accurate - don't set again!
    if ( error )
        return;

    assert( err > 0 && err < TIXML_ERROR_STRING_COUNT );
    error   = true;
    errorId = err;
    errorDesc = errorString[ errorId ];

    errorLocation.Clear();
    if ( pError && data )
    {
        data->Stamp( pError, encoding );
        errorLocation = data->Cursor();
    }
}


TiXmlNode* TiXmlNode::Identify( const char* p, TiXmlEncoding encoding )
{
    TiXmlNode* returnNode = 0;

    p = SkipWhiteSpace( p, encoding );
    if( !p || !*p || *p != '<' )
    {
        return 0;
    }

    TiXmlDocument* doc = GetDocument();
    p = SkipWhiteSpace( p, encoding );

    if ( !p || !*p )
    {
        return 0;
    }

    // What is this thing? 
    // - Elements start with a letter or underscore, but xml is reserved.
    // - Comments: <!--
    // - Decleration: <?xml
    // - Everthing else is unknown to tinyxml.
    //

    const char* xmlHeader = { "<?xml" };
    const char* commentHeader = { "<!--" };
    const char* dtdHeader = { "<!" };
    const char* cdataHeader = { "<![CDATA[" };

    if ( StringEqual( p, xmlHeader, true, encoding ) )
    {
        #ifdef DEBUG_PARSER
            TIXML_LOG( "XML parsing Declaration\n" );
        #endif
        returnNode = new TiXmlDeclaration();
    }
    else if ( StringEqual( p, commentHeader, false, encoding ) )
    {
        #ifdef DEBUG_PARSER
            TIXML_LOG( "XML parsing Comment\n" );
        #endif
        returnNode = new TiXmlComment();
    }
    else if ( StringEqual( p, cdataHeader, false, encoding ) )
    {
        #ifdef DEBUG_PARSER
            TIXML_LOG( "XML parsing CDATA\n" );
        #endif
        TiXmlText* text = new TiXmlText( "" );
        text->SetCDATA( true );
        returnNode = text;
    }
    else if ( StringEqual( p, dtdHeader, false, encoding ) )
    {
        #ifdef DEBUG_PARSER
            TIXML_LOG( "XML parsing Unknown(1)\n" );
        #endif
        returnNode = new TiXmlUnknown();
    }
    else if (    IsAlpha( *(p+1), encoding )
              || *(p+1) == '_' )
    {
        #ifdef DEBUG_PARSER
            TIXML_LOG( "XML parsing Element\n" );
        #endif
        returnNode = new TiXmlElement( "" );
    }
    else
    {
        #ifdef DEBUG_PARSER
            TIXML_LOG( "XML parsing Unknown(2)\n" );
        #endif
        returnNode = new TiXmlUnknown();
    }

    if ( returnNode )
    {
        // Set the parent, so it can report errors
        returnNode->parent = this;
    }
    else
    {
        if ( doc )
            doc->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
    }
    return returnNode;
}

const char* TiXmlElement::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    p = SkipWhiteSpace( p, encoding );
    TiXmlDocument* document = GetDocument();

    if ( !p || !*p )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, 0, 0, encoding );
        return 0;
    }

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }

    if ( *p != '<' )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, p, data, encoding );
        return 0;
    }

    p = SkipWhiteSpace( p+1, encoding );

    // Read the name.
    const char* pErr = p;

    p = ReadName( p, &value, encoding );
    if ( !p || !*p )
    {
        if ( document )	document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME, pErr, data, encoding );
        return 0;
    }

    TIXML_STRING endTag ("</");
    endTag += value;
    endTag += ">";

    // Check for and read attributes. Also look for an empty
    // tag or an end tag.
    while ( p && *p )
    {
        pErr = p;
        p = SkipWhiteSpace( p, encoding );
        if ( !p || !*p )
        {
            if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
            return 0;
        }
        if ( *p == '/' )
        {
            ++p;
            // Empty tag.
            if ( *p  != '>' )
            {
                if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY, p, data, encoding );		
                return 0;
            }
            return (p+1);
        }
        else if ( *p == '>' )
        {
            // Done with attributes (if there were any.)
            // Read the value -- which can include other
            // elements -- read the end tag, and return.
            ++p;
            p = ReadValue( p, data, encoding );		// Note this is an Element method, and will set the error if one happens.
            if ( !p || !*p )
                return 0;

            // We should find the end tag now
            if ( StringEqual( p, endTag.c_str(), false, encoding ) )
            {
                p += endTag.length();
                return p;
            }
            else
            {
                if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG, p, data, encoding );
                return 0;
            }
        }
        else
        {
            // Try to read an attribute:
            TiXmlAttribute* attrib = new TiXmlAttribute();
            if ( !attrib )
            {
                if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, pErr, data, encoding );
                return 0;
            }

            attrib->SetDocument( document );
            pErr = p;
            p = attrib->Parse( p, data, encoding );

            if ( !p || !*p )
            {
                if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, pErr, data, encoding );
                delete attrib;
                return 0;
            }

#ifdef DONT_ALLOW_MULTIPLE_ATTRIBUTES
            // Handle the strange case of double attributes:
            TiXmlAttribute* node = attributeSet.Find( attrib->Name() );
            if ( node )
            {
                node->SetValue( attrib->Value() );
                delete attrib;
                return 0;
            }
#endif
            attributeSet.Add( attrib );
        }
    }
    return p;
}


const char* TiXmlElement::ReadValue( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    TiXmlDocument* document = GetDocument();

    // Read in text and elements in any order.
    const char* pWithWhiteSpace = p;
    p = SkipWhiteSpace( p, encoding );

    while ( p && *p )
    {
        if ( *p != '<' )
        {
            // Take what we have, make a text element.
            TiXmlText* textNode = new TiXmlText( "" );

            if ( !textNode )
            {
                if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, encoding );
                    return 0;
            }

            if ( TiXmlBase::IsWhiteSpaceCondensed() )
            {
                p = textNode->Parse( p, data, encoding );
            }
            else
            {
                // Special case: we want to keep the white space
                // so that leading spaces aren't removed.
                p = textNode->Parse( pWithWhiteSpace, data, encoding );
            }

            if ( !textNode->Blank() )
                LinkEndChild( textNode );
            else
                delete textNode;
        } 
        else 
        {
            // We hit a '<'
            // Have we hit a new element or an end tag? This could also be
            // a TiXmlText in the "CDATA" style.
            if ( StringEqual( p, "</", false, encoding ) )
            {
                return p;
            }
            else
            {
                TiXmlNode* node = Identify( p, encoding );
                if ( node )
                {
                    p = node->Parse( p, data, encoding );
                    LinkEndChild( node );
                }				
                else
                {
                    return 0;
                }
            }
        }
        pWithWhiteSpace = p;
        p = SkipWhiteSpace( p, encoding );
    }

    if ( !p )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE, 0, 0, encoding );
    }	
    return p;
}


const char* TiXmlUnknown::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    TiXmlDocument* document = GetDocument();
    p = SkipWhiteSpace( p, encoding );

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }
    if ( !p || !*p || *p != '<' )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_UNKNOWN, p, data, encoding );
        return 0;
    }
    ++p;
    value = "";

    while ( p && *p && *p != '>' )
    {
        value += *p;
        ++p;
    }

    if ( !p )
    {
        if ( document )	document->SetError( TIXML_ERROR_PARSING_UNKNOWN, 0, 0, encoding );
    }
    if ( *p == '>' )
        return p+1;
    return p;
}

const char* TiXmlComment::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    TiXmlDocument* document = GetDocument();
    value = "";

    p = SkipWhiteSpace( p, encoding );

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }
    const char* startTag = "<!--";
    const char* endTag   = "-->";

    if ( !StringEqual( p, startTag, false, encoding ) )
    {
        document->SetError( TIXML_ERROR_PARSING_COMMENT, p, data, encoding );
        return 0;
    }
    p += strlen( startTag );
    p = ReadText( p, &value, false, endTag, false, encoding );
    return p;
}


const char* TiXmlAttribute::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p ) return 0;

//	int tabsize = 4;
//	if ( document )
//		tabsize = document->TabSize();

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }
    // Read the name, the '=' and the value.
    const char* pErr = p;
    p = ReadName( p, &name, encoding );
    if ( !p || !*p )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
        return 0;
    }
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p || *p != '=' )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
        return 0;
    }

    ++p;	// skip '='
    p = SkipWhiteSpace( p, encoding );
    if ( !p || !*p )
    {
        if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
        return 0;
    }
    
    const char* end;
    const char SINGLE_QUOTE = '\'';
    const char DOUBLE_QUOTE = '\"';

    if ( *p == SINGLE_QUOTE )
    {
        ++p;
        end = "\'";		// single quote in string
        p = ReadText( p, &value, false, end, false, encoding );
    }
    else if ( *p == DOUBLE_QUOTE )
    {
        ++p;
        end = "\"";		// double quote in string
        p = ReadText( p, &value, false, end, false, encoding );
    }
    else
    {
        // All attribute values should be in single or double quotes.
        // But this is such a common error that the parser will try
        // its best, even without them.
        value = "";
        while (    p && *p											// existence
                && !IsWhiteSpace( *p ) && *p != '\n' && *p != '\r'	// whitespace
                && *p != '/' && *p != '>' )							// tag end
        {
            if ( *p == SINGLE_QUOTE || *p == DOUBLE_QUOTE ) {
                // [ 1451649 ] Attribute values with trailing quotes not handled correctly
                // We did not have an opening quote but seem to have a 
                // closing one. Give up and throw an error.
                if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
                return 0;
            }
            value += *p;
            ++p;
        }
    }
    return p;
}

const char* TiXmlText::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding )
{
    value = "";
    TiXmlDocument* document = GetDocument();

    if ( data )
    {
        data->Stamp( p, encoding );
        location = data->Cursor();
    }

    const char* const startTag = "<![CDATA[";
    const char* const endTag   = "]]>";

    if ( cdata || StringEqual( p, startTag, false, encoding ) )
    {
        cdata = true;

        if ( !StringEqual( p, startTag, false, encoding ) )
        {
            document->SetError( TIXML_ERROR_PARSING_CDATA, p, data, encoding );
            return 0;
        }
        p += strlen( startTag );

        // Keep all the white space, ignore the encoding, etc.
        while (	   p && *p
                && !StringEqual( p, endTag, false, encoding )
              )
        {
            value += *p;
            ++p;
        }

        TIXML_STRING dummy; 
        p = ReadText( p, &dummy, false, endTag, false, encoding );
        return p;
    }
    else
    {
        bool ignoreWhite = true;

        const char* end = "<";
        p = ReadText( p, &value, ignoreWhite, end, false, encoding );
        if ( p )
            return p-1;	// don't truncate the '<'
        return 0;
    }
}

const char* TiXmlDeclaration::Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding _encoding )
{
    p = SkipWhiteSpace( p, _encoding );
    // Find the beginning, find the end, and look for
    // the stuff in-between.
    TiXmlDocument* document = GetDocument();
    if ( !p || !*p || !StringEqual( p, "<?xml", true, _encoding ) )
    {
        if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION, 0, 0, _encoding );
        return 0;
    }
    if ( data )
    {
        data->Stamp( p, _encoding );
        location = data->Cursor();
    }
    p += 5;

    version = "";
    encoding = "";
    standalone = "";

    while ( p && *p )
    {
        if ( *p == '>' )
        {
            ++p;
            return p;
        }

        p = SkipWhiteSpace( p, _encoding );
        if ( StringEqual( p, "version", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );		
            version = attrib.Value();
        }
        else if ( StringEqual( p, "encoding", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );		
            encoding = attrib.Value();
        }
        else if ( StringEqual( p, "standalone", true, _encoding ) )
        {
            TiXmlAttribute attrib;
            p = attrib.Parse( p, data, _encoding );		
            standalone = attrib.Value();
        }
        else
        {
            // Read over whatever it is.
            while( p && *p && *p != '>' && !IsWhiteSpace( *p ) )
                ++p;
        }
    }
    return 0;
}

bool TiXmlText::Blank() const
{
    for ( unsigned i=0; i<value.length(); i++ )
        if ( !IsWhiteSpace( value[i] ) )
            return false;
    return true;
}

