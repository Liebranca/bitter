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

PE_COMMTED   = 0x01;
PE_ESCAPED   = 0x02;
PE_LITERAL   = 0x04;

PE_INSLEX    = 0x08;
PE_INSDEF    = 0x10;

LEX          = {};
perd         = nil;

#   ---     ---     ---     ---     ---

class PEEX
    attr_accessor :type, :tokens, :idex, :origin, :trans
    def initialize
        @type   = "";
        @idex   = 0;

        @origin = "";
        @trans  = "";

        @tokens = {};

        @upk_namelist = ->(t, prev, sub, k) {

            if(!(prev.empty?))
                i=1; names=sub.tokens[k].split(',')
                ut=""; names.each { |n|;
                    sep=', '; if(i==names.length);
                        sep='';

                    end; ut << "#{prev} #{n}#{sep}"; i+=1;

                }; return ut;

            end; return "";

        };

    end;

#   ---     ---     ---     ---     ---

    def upkd(_perd, t)

        ut="";
        if(@tokens[t].kind_of?(Array))
            lt="";
            @tokens[t].each { |sub|; prev="";
                sub.tokens.each_key { |k|;

#   ---     ---     ---     ---     ---

                    tr=sub.tokens[k];
                    if(_perd.defs.include?(k))
                        if(_perd.defs[k].include?(tr))
                            tr=_perd.defs[k][tr];

                        end;

                    elsif(_perd.defs.include?(sub.type))
                        if(_perd.defs[sub.type].include?(tr))
                            tr=_perd.defs[sub.type][tr];

                        end;

                    end;

                    if(t=="#arglist")
                        if(lt=="namelist")
                            ut << ", ";

                        end; ut << @upk_namelist.call(t, prev, sub, k);
                        prev=tr;

#   ---     ---     ---     ---     ---

                    else
                        ut << "#{tr} ";

                    end; lt=k;
                };
            };

#   ---     ---     ---     ---     ---

        else
            ut=@tokens[t];

        end; return ut;

    end;

#   ---     ---     ---     ---     ---

    def inspect

        if(@type[0]!='#')
            sep=""; for i in 0..16-("#BLOCK").length
                sep << ' ';

            end; puts "#BLOCK#{sep} >> #{@type}\n\n";

        else
            sep=""; for i in 0..16-("breakdown").length
                sep << ' ';

            end; puts "\nbreakdown#{sep} >> #{@type}";

        end;

#   ---     ---     ---     ---     ---

        @tokens.each_key { |t|

            if(@tokens[t].kind_of?(Array))
                @tokens[t].each { |sub|
                    sub.inspect; puts;

                };

#   ---     ---     ---     ---     ---

            else
                sep=""; for i in 0..16-t.length
                    sep << ' ';

                end; puts "#{t}#{sep} >> #{@tokens[t]}";

            end;

        };
    end;

#   ---     ---     ---     ---     ---

    def translate(_perd)

        pos = _perd.trans[@type];
        pos.each { |entry|;

            i, b, e = entry.split('|#');
            i=i.to_i; if(i!=@idex); next; end;

            chktag, op, lit = b.split(' ');
            avail           = 1;

            if(@tokens.include?(chktag))
                avail=eval("'#{@tokens[chktag]}'#{op}'#{lit}'")

            end;

#   ---     ---     ---     ---     ---

            if(avail==1)
                e.split(' ').each { |t|;

                    sep=' ';
                    if(@tokens.include?(t))
                        @trans << "#{self.upkd(_perd, t)}#{sep}";

                    elsif( !(_perd.rules.include?(t)) \
                        && !(_perd.keys.include?(t) ) )
                        @trans << "#{t}#{sep}";

                    end;
                }; break;
            end;
        };
    end;

#   ---     ---     ---     ---     ---

    def sbs
        puts "$: #{@origin}";
        puts "C: #{@trans}";

    end;
end;

#   ---     ---     ---     ---     ---

class READER
    attr_accessor :act, :keys, :rules, :defs, :forms, :trans
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

        @trans  = {};
        @transi = 0;
        @defs   = {};
        @mstr   = {};

        @onins  = nil;

        @peopcd = nil;
        @peskip = 0;

        @peargc = 0;
        @peargv = nil;

    end;

#   ---     ---     ---     ---     ---

    def find_reserved(mt)
        @keys.each_key { |k|;
            if(@keys[k].include?(mt)); return k; end;

        }; return "";

    end;

#   ---     ---     ---     ---     ---

    def brkdown_regex(r, s, ex)

        tags=r.split(' ');
        tags.each { |t|; mt="";

            if(@keys.include?(t))
                mt=s.match(@keys[t]).to_s;

            elsif(@rules.include?(t))
                if(t[0]=='#')
                    mt=s.match(@rules[t][0]).to_s;

                else
                    mt=s.match(@rules[t]).to_s;
                    if(!(["flow", "vfunct", "type", "typeflag"].include?(ex.type)))
                        if(!(mt.match(@rules["#reserved"][0]).to_s.empty?))
                            ex.type=find_reserved(mt);

                        end;
                    end;

                end;

            elsif(s[0..t.length]==t)
                mt=t;

            end;

            if(!(mt.empty?))

                s=s[mt.length..-1];
                while(s[0]==' ')
                    s=s[1..-1];

                end;

                if(t[0]=='#')

                    ex.tokens[t] = [];
                    while(!(mt.empty?))
                        nx=PEEX.new; nx.type=t;
                        ex.tokens[t].append(nx);

                        mt=brkdown_regex(@rules[t][1], mt, ex.tokens[t][-1]);

                    end;

                else
                    ex.tokens[t] = mt;

                end;
            end;

        }; return s;
    end;

#   ---     ---     ---     ---     ---

    def parse(s)

        ex=PEEX.new;

        while(s.include?("\n")); s["\n"]=' '; end;
        i=0; @forms.each_key { |k|

            @forms[k].each { |f|
                m=s.match(eval("/#{f[0]}/"));
                if(m.to_s==s);

                    ex.type=k; ex.idex=i; ex.origin=s;
                    brkdown_regex(f[1], s.squeeze(' '), ex);
                    ex.translate(self);

                    return ex;

                end; i+=1;

            }; i=0;
        }; return nil;

    end;

#   ---     ---     ---     ---     ---

    def stdef(val)

        @state    &=~PE_INSDEF; 

        @mstr      = "";
        @defs[val] = {};

        @onins = ->(t) {

            if((@state&PE_INSDEF)==0)
                if(t=="is")
                    @state|=PE_INSDEF;
                    @defs["#{val}"][@mstr] = "";

                else
                    sep=' '; if(@mstr.empty?); sep=''; end;
                    @mstr << sep+t;

                end;

            else
                if(t==";")
                    @state&=~PE_INSDEF;
                    @mstr  = "";

                else
                    sep=' '; if(@defs["#{val}"][@mstr].empty?); sep=''; end;
                    @defs["#{val}"][@mstr] << sep+t;

                end;
            end;

        };
    end;

#   ---     ---     ---     ---     ---

    def sttrans(val)
        @trans[val] = [];

        @transi     = 0;
        @mstr       = "";

        @onins = ->(t) {

            sep=' ';
            if(@mstr.empty?)
                @trans["#{val}"].append("");
                sep='';

            end;

            if(@transi==0 && t=='(')
                @mstr << "|#"; @transi += 1;

            elsif(@transi==1 && t==')')
                @mstr << "\b|#"; @transi += 1;

            elsif(t==';')
                @trans["#{val}"][-1] = @mstr[0..-2];
                @mstr=""; @transi=0;

            else
                @mstr << "#{t}#{sep}";

            end;

        };
    end;

#   ---     ---     ---     ---     ---

    def stcol(wr, val)

        if(val[0]=='#')
            eval("@#{wr}['#{val}'] = ['','']");

        else
            eval("@#{wr}['#{val}'] = ''");

        end;

#   ---     ---     ---     ---     ---

        @onins = ->(t) {

            sep="|"; if(@cchar==';' || "#{val}".include?('#')); sep=''; end;
            if("#{wr}"=="rules")
                if(t[0]=='<' && t[-1]=='>')
                    t=t[1..-2];
                    if(@rules.include?(t))
                        tag=@rules[t][0..-2];

                    else
                        tag=@keys[t][0..-1];

                    end;

#   ---     ---     ---     ---     ---

                    if("#{val[0]}"=='#')
                        @rules["#{val}"][0] << "#{tag}";
                        @rules["#{val}"][1] << "#{t} ";

                    else
                        @rules["#{val}"] << "#{tag}";

                    end;

#   ---     ---     ---     ---     ---

                else
                    if("#{val[0]}"=='#')
                        @rules["#{val}"][0] << "#{t}";
                        @rules["#{val}"][1] << "#{t} ";

                    else
                        @rules["#{val}"] << "#{t}#{sep}";

                    end;
                end;

            else
                @keys["#{val}"] << "#{t}#{sep}";

            end;
        };
    end;

#   ---     ---     ---     ---     ---

    def stform(val)
        @forms[val] = [["", ""]]
        @formi      = 0;

        @onins      = ->(t) {

            if(t[0]=='<' && t[-1]=='>')     # is tag
                tag=t[1..-2];

#   ---     ---     ---     ---     ---

                if   (@keys.include?(tag))
                    forms["#{val}"][@formi][0] << "(#{@keys[tag]})"
                    forms["#{val}"][@formi][1] << "#{tag} ";

                elsif(@rules.include?(tag))
                    if(tag[0]=='#')
                        forms["#{val}"][@formi][0] << "#{@rules[tag][0][0..-1]}"

                    else
                        forms["#{val}"][@formi][0] << "#{@rules[tag][0..-2]}"

                    end; forms["#{val}"][@formi][1] << "#{tag} ";

                else
                    puts "Invalid tag #{t} in form <#{val}>"

                end;

            elsif(t==';')
                forms["#{val}"][@formi][1]=forms["#{val}"][@formi][1][0..-2];
                @formi+=1; forms["#{val}"].append(["",""]);

            else
                forms["#{val}"][@formi][0] << t;
                forms["#{val}"][@formi][1] << "#{t} ";

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
                                        º1
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

LEX["("    ]=PEOP.new(0x00000000, ->(x) { perd.RQ_OPEN(x)          } );
LEX[")"    ]=PEOP.new(0x00000000, ->(x) { perd.RQ_CLSE(x)          } );
LEX["{"    ]=PEOP.new(0x00000000, ->(x) { perd.SQ_OPEN(x)          } );
LEX["}"    ]=PEOP.new(0x00000000, ->(x) { perd.SQ_CLSE(x)          } );

LEX["itr"  ]=PEOP.new(0x00000002, nil                                );
LEX["lex"  ]=PEOP.new(0x00000000, ->(x) { perd.lexbit(x)           } );
LEX["key"  ]=PEOP.new(0x00000001, ->(x) { perd.stcol("keys",  x)   } );
LEX["rule" ]=PEOP.new(0x00000001, ->(x) { perd.stcol("rules", x)   } );
LEX["form" ]=PEOP.new(0x00000001, ->(x) { perd.stform(x)           } );
LEX["def"  ]=PEOP.new(0x00000001, ->(x) { perd.stdef(x)            } );
LEX["trans"]=PEOP.new(0x00000001, ->(x) { perd.sttrans(x)          } );


LEX["chk_in"] = ->(var_name) {
    Module.constants.include?(var_name.to_sym);

};

#   ---     ---     ---     ---     ---

fpath      = File.join(__dir__, "lex.pe");

tokens     = [];
tk         = "";

#   ---     ---     ---     ---     ---

File.foreach(fpath) { |line|

    chars = line.split('');
    i     = 0;
    abort = 0;

    chars.each { |c|; i+=1;

        perd.rdchk(c, chars[i]);
        if(perd.act!=0);
            if(perd.act==1);
                perd.act=0; break;

            end;
        end;

#   ---     ---     ---     ---     ---

    }; if(abort==1); break; end;
};

#   ---     ---     ---     ---     ---
#test block

s="if(a) then (b) {"\
  "int c = 0x4A;"\
  "}"\

toks=[]; tk="";

s.split('').each { |c|;
    tk << c;
    if("{};".include?(c))
        if(c==';')
             tk=tk[0..-2];

        end; toks.append(tk); tk="";

    end;

};

#   ---     ---     ---     ---     ---

i=0; toks.each { |t|;
    ex=perd.parse(t);

    if(ex!=nil)
        puts "\n# --  EXPR "+i.to_s+" ---   --  ---  --\n\n\n";
        ex.inspect;
        ex.sbs; i+=1;

    end;

};

#   ---     ---     ---     ---     ---