#*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/#
#*    PESOSCRIPT                            #
#                                           #
#     -ruby implementation of $ intprt      #
#                                           #
#/ *   ---     ---     ---     ---     ---  #
#*    LIBRE SOFTWARE                        #
#                                           #
#     Licenced under GNU GPL3               #
#     be a bro and inherit                  #
#                                           #
#/ *   ---     ---     ---     ---     ---  #
#* CONTRIBUTORS                             #
#     lyeb,                                 #
#                                           #
#*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/#

require 'io/console' #getch

#   ---     ---     ---     ---     ---

PE_COMMTED = 0x01;
PE_ESCAPED = 0x02;
PE_LITERAL = 0x04;

PE_INSLEX  = 0x08;

LEX        = {};

#   ---     ---     ---     ---     ---

class READER
    attr_accessor :act, :tokens, :keys, :rules, :forms
    def initialize

        @lvl    = 0;
        @elm    = 0;
        @state  = 0;
        @skip   = 0;

        @ltokn  = "";
        @ctokn  = "";

        @lchar  = '';
        @cchar  = '';
        @nchar  = '';

        @act    = 0;
        @tokens = [];

        @keys   = {};
        @rules  = {};
        @forms  = {};
        @formi  = 0;

        @onins  = nil;

        @peopcd = nil;
        @peskip = 0;

        @peargc = 0;
        @peargv = nil;

    end;

#   ---     ---     ---     ---     ---

    def parse(s)

        i=0; @forms.each_key { |k|
            @forms[k].each { |f|
                m=s.match(eval("/#{f}/"));
                if(m.to_s==s);
                    puts "#{s} is a form ##{i} #{k};\nMatching reg /#{f}/";
                    return 1;

                end; i+=1;

            };
        }; return 0;

    end;

#   ---     ---     ---     ---     ---

    def stcol(wr, val)
        eval("@#{wr}['#{val}'] = ''");
        @onins = ->(t) {
            sep="|"; if(@cchar==';'); sep=''; end;
            eval("@#{wr}['#{val}'] << '\\b#{t}\\b#{sep}';");

        }

    end;

    def stform(val)
        @forms[val] = [""]
        @formi      = 0;

        @onins      = ->(t) {

            if(t[0]=='<' && t[-1]=='>')     # is tag
                tag=t[1..-2];

#   ---     ---     ---     ---     ---

                if   (@keys.include?(tag))
                    forms["#{val}"][@formi] << "(#{@keys[tag]}) "

                elsif(@rules.include?(tag))
                    forms["#{val}"][@formi] << "#{@rules[tag][0..-2]} "

                else
                    puts "Invalid tag #{t} in form <#{val}>"

                end;

            elsif(t==';')
                forms["#{val}"][@formi]=@forms["#{val}"][@formi][0..-2];
                @formi+=1; forms["#{val}"].append("");

            end;

        }

    end;

#   ---     ---     ---     ---     ---

    def bitset(x)
        if(x); return "|="; end;
        return "&=~";

    end;

    def lexbit(x)
        eval("@state #{self.bitset(x)} PE_INSLEX");
    end;

#   ---     ---     ---     ---     ---

    def rdchk(cc, nc)

        @lchar=@cchar;

        @cchar=cc;
        @nchar=nc;

        if   (self.iscomment == 1); return;
        elsif(self.isliteral == 1); return;

        elsif((@state&PE_LITERAL)!=0); self.rdchr; return;

        elsif(self.igdchr    == 1); return;
        elsif(self.eoe       == 1); return;
        elsif(self.scopers   == 1); return;
        elsif(self.escaper   == 1); return; end;

        self.rdchr;

    end;

#   ---     ---     ---     ---     ---

    def rdchr

        if(" \t\n\r".include?(@cchar))
            self.tkins;
            @state&=~PE_LITERAL;
            return;

        end;

        if(@skip==1)
            @skip=0;
            return;

        end;

        if( ( @cchar!='\\'               ) \
        ||  (@cchar=='\\' && @lchar=='\\') )
            @ctokn << @cchar;

        end;

    end;

#   ---     ---     ---     ---     ---

    def isliteral
        if(@cchar=='\\' && (@state&PE_LITERAL)==0)
            @state|=PE_LITERAL;
            return 1;

        end; return 0;

    end;

#   ---     ---     ---     ---     ---

    def iscomment
        if(@cchar=='/')
            if(@nchar=='*');
                @state|= PE_COMMTED;
                @skip  = 1;
                return 1;

            elsif(@nchar=='/');
                @act   = 1;
                return 1;

            end;

#   ---     ---     ---     ---     ---

        elsif(@cchar=='*' && @nchar=='/')
            @state &=~PE_COMMTED;
            @skip   = 1;
            return 1;

        end; return 0;

    end;

#   ---     ---     ---     ---     ---

    def igdchr

        if(@skip==1)
            @skip = (" \t\n\r".include?(@cchar));
            return 1;

        elsif((@state&PE_COMMTED)!=0);
            @skip = 1;
            return 1;

        end; return 0;

    end;

#   ---     ---     ---     ---     ---

    def tkins
        if(!(@ctokn.empty?));

            if((@state&PE_ESCAPED)!=0)
                if("%/".include?(@ctokn[0]))
                    LEX[@ctokn[1..-1]].fun.call(@ctokn[0]=='%');

                elsif(@peopcd==nil && LEX.key?(@ctokn))
                    @peopcd=LEX[@ctokn];
                    @peargc=@peopcd.data&0x000000FF;
                    @peargv=[];
                    @peskip=1;

                end;

#   ---     ---     ---     ---     ---

                if(@peopcd!=nil && @peskip==0)

                    if(@peargc>0)
                        @peargv.append(@ctokn);
                        @peargc-=1;

                    end;

                end;

            end;

#   ---     ---     ---     ---     ---

            @tokens.append(@ctokn);
            if(@onins!=nil && @peskip==0);
                @onins.call(@ctokn);

            end; @peskip=0;

            #puts @ctokn;
            #STDIN.getch;

            @ltokn=@ctokn;
            @ctokn="";

        end;

    end;

#   ---     ---     ---     ---     ---

    def eoe
        if(@cchar==';')
            if(@nchar=='>')
                self.tkins;
                @state&=~PE_ESCAPED;
                @ctokn=";>"; self.tkins;

                if(@peopcd!=nil)
                    @peopcd.fun.call(*@peargv);

                end; @peopcd=nil;

                @peargc=0;
                @peargv=nil;

                @skip=1;

            elsif((@state&PE_LITERAL)==0)
                self.tkins;
                @onins=nil;
                @ctokn="#EE"; self.tkins;

            end;

            return 1;

        end; return 0;

    end;

#   ---     ---     ---     ---     ---

    def scopers
        if("(){}".include?(@cchar))
            self.tkins;
            @ctokn=LEX[@cchar].fun.call(@cchar); self.tkins;

            return 1;

        end; return 0;

    end;

#   ---     ---     ---     ---     ---

    def escaper
        if(@cchar=="$")
            if(@nchar==':')
                self.tkins;
                @state|=PE_ESCAPED;
                @ctokn="$:"; self.tkins;
                @skip=1;

                return 1;

            end;

        end; return 0;

    end;

#   ---     ---     ---     ---     ---

    def RQ_OPEN(c)
        s="#{c} #{@elm}"
        @elm+=1; return c;

    end;

    def RQ_CLSE(c)
        @elm-=1;
        return c; #"#{c} #{@elm}"

    end;

#   ---     ---     ---     ---     ---

    def SQ_OPEN(c)
        s="#{c} #{@lvl}"
        @lvl+=1; return c;

    end;

    def SQ_CLSE(c)
        @lvl-=1;
        return c; #"#{c} #{@lvl}"

    end;

#   ---     ---     ---     ---     ---

end; perd=READER.new;

#   ---     ---     ---     ---     ---

PEOP=Struct.new(:data, :fun) do
    def argcount
        return data & 0x000000FF;
    end;

end;

#   ---     ---     ---     ---     ---

LEX["("   ]=PEOP.new(0x00000000, ->(x) { perd.RQ_OPEN(x)          } );
LEX[")"   ]=PEOP.new(0x00000000, ->(x) { perd.RQ_CLSE(x)          } );
LEX["{"   ]=PEOP.new(0x00000000, ->(x) { perd.SQ_OPEN(x)          } );
LEX["}"   ]=PEOP.new(0x00000000, ->(x) { perd.SQ_CLSE(x)          } );

LEX["itr" ]=PEOP.new(0x00000002, nil                                );
LEX["lex" ]=PEOP.new(0x00000000, ->(x) { perd.lexbit(x)           } );
LEX["key" ]=PEOP.new(0x00000001, ->(x) { perd.stcol("keys",  x)   } );
LEX["rule"]=PEOP.new(0x00000001, ->(x) { perd.stcol("rules", x)   } );
LEX["form"]=PEOP.new(0x00000001, ->(x) { perd.stform(x)  } );

LEX["chk_in"] = ->(var_name) {
    Module.constants.include?(var_name.to_sym);

};

#   ---     ---     ---     ---     ---

fpath      = File.join(__dir__, "lex.pe");

tokens     = [];
tk         = "";

#   ---     ---     ---     ---     ---

File.foreach(fpath) { |pe|
    chars = pe.split('');
    i     = 0;
    abort = 0;

    chars.each { |c|; i+=1;

        perd.rdchk(c, chars[i]);
        if(perd.act!=0);
            if(perd.act==1); perd.act=0; break; end;
        end;

#   ---     ---     ---     ---     ---

    }; if(abort==1); break; end;
};

#   ---     ---     ---     ---     ---