job('hpdf_table_ub18') {
  authenticationToken('hpdf_table')
  scm {
    git {
      remote {
        name('origin')
        url('https://github.com/johan162/hpdf_table.git')
        branch('*/master')
      }
    }
  }
  properties {
    githubProjectUrl('https://github.com/johan162/hpdf_table')
  }  
  label('ubuntu18')
  logRotator {
    numToKeep(3)
  }
  wrappers {
    timestamps()
    preBuildCleanup()
  }  
  steps {
    shell("./scripts/bootstrap.sh && make -j4")
  }
}

job('hpdf_table_ub20') {
  authenticationToken('hpdf_table')
  scm {
    git {
      remote {
        name('origin')
        url('https://github.com/johan162/hpdf_table.git')
        branch('*/master')
      }
    }
  }
  properties {
    githubProjectUrl('https://github.com/johan162/hpdf_table')
  }  
  label('ubuntu20')
  logRotator {
    numToKeep(3)
  }
  wrappers {
    timestamps()
    preBuildCleanup()
  }  
  steps {
    shell("./scripts/bootstrap.sh && make -j4")
  }
}

job('hpdf_table_ub22') {
  authenticationToken('hpdf_table')
  scm {
    git {
      remote {
        name('origin')
        url('https://github.com/johan162/hpdf_table.git')
        branch('*/master')
      }
    }
  }
  properties {
    githubProjectUrl('https://github.com/johan162/hpdf_table')
  }  
  label('ubuntu22')
  logRotator {
    numToKeep(3)
  }
  wrappers {
    timestamps()
    preBuildCleanup()
  }  
  steps {
    shell("./scripts/bootstrap.sh && make -j4")
  }
}