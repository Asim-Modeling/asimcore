:
eval 'exec perl -w "$0" ${1+"$@"}'
       if 0;

#############################################################################
# Copyright (c) 2014, Intel Corporation
#
# All rights reserved.
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are 
# met:
# 
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright 
#   notice, this list of conditions and the following disclaimer in the 
#   documentation and/or other materials provided with the distribution.
# - Neither the name of the Intel Corporation nor the names of its 
#   contributors may be used to endorse or promote products derived from 
#   this software without specific prior written permission.
#  
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#############################################################################
# 
# Brief:  Create default tar
# 

#
# Convert input file inFileName into a sequence of C code that generates
# the contents of the file as string in char* buffer bufName. Space to
# hold the string in the buffer is dynamically allocated.
# The characters '\' and '"' are quoted to pass through correctly.
#
sub BuildCStringFromFile {
  my $inFileName = shift;
  my $bufName = shift;

  my $inLine;
  my $outLine;
  my @out;
  my $outSize = 0;

  open( IN, "<$inFileName" )
    or die "Can't open $inFileName for read\n$!";

  while ($inLine = <IN>) {
    $lineLength = length( $inLine );
    chomp $inLine;
    # some characters need quoting
    #
    # replace '\' with '\\'
    $inLine =~ s/[\\]/\\\\/g;
    # replace '"' with '\"'
    $inLine =~ s/["]/\\"/g;
    # remove DOS end-of-line crap @!#$%
    # $inLine =~ s/\x0d//;
    $inLine =~ s/[\0]/\\000/g;

    $outLine = "$inLine\\n";
    $oldSize = $outSize;
    $outSize += $lineLength;
    push @out, "\tmemcpy($bufName + $oldSize, \"$outLine\", $lineLength);\n";
  }
  close( IN )
    or die "Error closing $inFileName\n$!";

  return $outSize, @out;
}


#
# Create a source file defining the default (built-in) tar. This
# basically creates a very long string containing the whole TCL code of
# the default tar for this model.
#
sub CreateDefaultWorkbench {
  my @inFiles = shift;

  #my $outFileName = "prova_tar.cpp";
  my $strName = "tar";

  # Open output file for writing and output the header info.
  #open( OUT, ">$outFileName" )
  #  or die "Can't open $outFileName for write\n$!";

  my @tar;
  my $tarSize = 0;

  foreach $file (@inFiles) {
    my ($size, @strings) = BuildCStringFromFile( $file, $strName );
    push @tar, @strings;
    $tarSize += $size;
  }

  # generate header
  print "/*************************************************\n";
  print " * This source file is automatically generated\n";
  print " * to define the default TAR file\n";
  print " *************************************************/\n";
  print "\n";
  print "#include <string.h>\n";
  print "#include \"asim/dralServer.h\"\n";
  print "\n";
  print "char * DRAL_SERVER_CLASS::GetTar (UINT32 * size) {\n";
  print "\tconst unsigned int tarSize = $tarSize;\n";
  print "\t*size = tarSize;\n";
  print "\tchar *$strName = new char[tarSize];\n";
  print "\t*$strName = 0;\n";

  # ... contents ...
  print @tar;

  # ... trailer
  print "\treturn($strName);\n";
  print "}\n";

  #close( OUT )
  #  or die "Error closing $outFileName\n$!";
}

#----------------------------------------------------------------------------
# main stuff
#----------------------------------------------------------------------------

@tarFiles = @ARGV;

printf STDERR "creating default tar for @tarFiles\n";
CreateDefaultWorkbench(@tarFiles);

# vim:set filetype=perl:
