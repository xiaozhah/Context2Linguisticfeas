$| = 1;

$labdir = "./fulllab";
$que    = "./questions.hed";
$outdir = "./fulllab_answers_perl";
mkdir $outdir, 0755;
opendir DIR, $labdir;
$i = 1;
Get_Ques2Regular($que);
foreach $_ ( sort {$a <=> $b} readdir DIR ) 
{
  if (/(.*)\.lab/) 
  {
    $filename = $1;
    print $filename, "\t", "$i\n";
    $i++;
    $lab = sprintf("$labdir/$filename.lab");
    $out = sprintf("$outdir/$filename.dat");
    feature( $lab, $que, $out );
  }
}
closedir DIR;

sub Get_Ques2Regular
{
  open( question, "<@_[0]" ) or die "questions.hed can't be open";
  %Ques2Regular;
  while ( $line_question = <question> )
  {
    if ( $line_question =~ /^\s*$/ )
    {
        next;
    }
    else 
    {
     if ( $line_question =~ /\{(.*)\}/ ) 
      {
        @phoneall = split( /,/, $1 );
      }
      foreach $phoneone (@phoneall) 
      {
        $original_phoneone = $phoneone;
        $phoneone =~ s/\*/\.\*/g;    #(*->.*)
        $phoneone =~ s/\?/\./g;      #(?->. )
        $phoneone =~ s/\$/\\\$/g;    #($->\$)
        $phoneone =~ s/\+/\\\+/g;    #(+->\+)
        $phoneone =~ s/\|/\\\|/g;    #(|->\|)
        $phoneone =~ s/\^/\\\^/g;    #(^->\^)
        $phoneone =~ s/^([a-z])/\^$1/; # such as m-* -> ^m-*
        $phoneone =~ s/([^*])$/$1\$/; # such as m-* -> ^m-*
        $phoneone = qr/$phoneone/i; # Compile regex pattern
        $Ques2Regular{$original_phoneone} = $phoneone unless exists $Ques2Regular{$original_phoneone};
      }
    }
  }
  close(question);
}

sub feature 
{
  open( label,    "<@_[0]" ) or die "00000001.lab can't be open";
  open( question, "<@_[1]" ) or die "questions.hed can't be open";
  open( output,   ">@_[2]" ) or die "output.txt is null";
  binmode output;

  while ( $line_label = <label> ) 
  {
    seek question, 0, 0;
    $line_label =~ s/\s+$//; # similar to line_label.rstrip() in Python
    while ( $line_question = <question> ) 
    {
      if ( $line_question =~ /^\s*$/ )
      {
          next;
      }
      else {
          if ( $line_question =~ /\{(.*)\}/ ) 
          {
              @phoneall = split( /,/, $1 );
          }
          $temp = 0;
          foreach $phoneone (@phoneall) 
          {
              $phoneone = $Ques2Regular{$phoneone};
              if ( $line_label =~ /$phoneone/ ) 
              {
                  $temp = 1;
                  print output pack( "f", 1 );
                  last;
              }
          }
          if ( $temp == 0 ) 
          {
              print output pack( "f", 0 );
          }
        }
    }
  }
  close(label);
  close(question);
  close(output);
}
