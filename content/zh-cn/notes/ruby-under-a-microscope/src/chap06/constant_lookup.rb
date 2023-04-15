#!/usr/bin/env ruby

FIND_ME = "Global"

puts "=== Base class goes first ==="
module BaseClassGoesFirst
  class Base
    FIND_ME = "Base"
  end

  module ParentLexicalScope
    #FIND_ME = "ParentLexicalScope"

    module ChildLexicalScope
      class Derived < Base
        p FIND_ME
      end
    end
  end
end

puts "=== Modules (Lexical Scopes) go first ==="
module ModuleGoesFirst
  module ParentLexicalScope
    #FIND_ME = "ParentLexicalScope"

    module ChildLexicalScope
      class Base
        FIND_ME = "Base"
      end

      class Derived < Base
        p FIND_ME
      end
    end
  end
end

