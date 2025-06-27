def getCron(){
    if (env.JENKINS_URL == "https://jenkins-lset.appli.gre-rke2.intra.cea.fr/"){
        return env.BRANCH_NAME == 'master' ? '@daily' :'H 5 * * *'
    }else{
        return '@monthly'
    }
}

def send_test_report() {

    
    script{
        //si un utilisateur lance le build il est le seul à recevoir un mail
        if ( currentBuild.rawBuild.getCauses()[0].toString().contains('UserIdCause') ){
            emailext (
                recipientProviders: [requestor()],                
                subject: "[NRT Cairn] Cairn pipeline",
                body: readFile("tests/scripts/default_email.html"),
                mimeType: 'text/html'
            );
        //sinon si c'est un lancement automatique on fait avec la liste de base
        } else {
            emailext (
                to: 'quentin.demarque@cea.fr,stephanie.crevon@cea.fr, ali.kassem@cea.fr,joel.chebassier@cea.fr,youcef.djebour@cea.fr,thibaut.wissocq@cea.fr,gabriele.leoncini@cea.fr,pimprenelle.parmentier@cea.fr',
                subject: "[NRT Cairn] Cairn pipeline",
                body: readFile("tests/scripts/default_email.html"),
                mimeType: 'text/html'
            );
        }
    }
}

pipeline {
    agent {
        node {
            label "${params.agent}"
            //TBReviewed
            customWorkspace "F:\\Persee_Project\\${params.PREFIX_WS}Core-${CAIRN_GIT_BRANCH}_Gui-${params.CAIRNGUI_GIT_BRANCH}_MIP-${MIPMODELER_GIT_BRANCH}_psm-${params.PSM_GIT_BRANCH}\\"
        }
    }

    triggers {
        cron(getCron())
    }

    parameters {
        string(name: 'agent', defaultValue: 'grep773', description: 'nom de l\'agent soit grep773 soit grep1003')        
        string(name: 'installeur_pegase', defaultValue: '', description: 'nom de l\'installeur Pegase, par defaut recupere le fichier le plus recent dans D:\\A_VERSION_PEGASE')
        string(name: 'CAIRNGUI_GIT_BRANCH', defaultValue: 'main', description: 'nom de la branche pour PERSEEGUI, ne pas changer pour execution par defaut sur main')
        string(name: 'CAIRN_GIT_BRANCH', defaultValue: 'main', description: 'nom de la branche pour PERSEE, ne pas changer pour execution par defaut')
        string(name: 'MIPMODELER_GIT_BRANCH', defaultValue: 'master', description: 'nom de la branche pour MIPMODELER, ne pas changer pour execution par defaut')
        string(name: 'PREFIX_WS', defaultValue: '', description: 'prefix pour le répertoire de travail')
        string(name: 'PSM_GIT_BRANCH', defaultValue: 'main', description: 'nom de la branche pour private submodel, ne pas changer pour execution par defaut')
    }

    environment {
        CAIRN_APP = "${WORKSPACE}"
        PEGASE_APP = "${WORKSPACE}"
    }


    stages {

        stage("set variables"){
            steps {
                script {
                        currentBuild.displayName = "#${BUILD_NUMBER}-${params.agent}"
                }
            }
        }
        //update les submodules manuellement
        stage('gestion submodules and checkouts'){
            steps{
                //change gui branch if needed then init the submodules
                bat "if NOT '${params.CAIRN_GIT_BRANCH}' == '' (git -c http.sslVerify=false reset --hard && git clean -fdx && git checkout ${params.CAIRN_GIT_BRANCH} && git pull origin ${params.CAIRN_GIT_BRANCH})"
                bat "git -c http.sslVerify=false submodule update --init --force --recursive"

                bat "if NOT '${params.CAIRNGUI_GIT_BRANCH}' == '' (cd gui && git -c http.sslVerify=false reset --hard && git clean -fdx  && git checkout ${params.CAIRNGUI_GIT_BRANCH} && git pull && git submodule update --init --force --remote)"
				
				bat "if NOT '${params.PSM_GIT_BRANCH}' == '' (cd src\\privateModels && git -c http.sslVerify=false reset --hard && git clean -fdx  && git checkout ${params.PSM_GIT_BRANCH}  && git pull)"

                bat "if NOT '${params.MIPMODELER_GIT_BRANCH}' == '' (cd lib\\MIPModeler && git -c http.sslVerify=false reset --hard && git clean -fdx && git checkout ${params.MIPMODELER_GIT_BRANCH} && git pull)"
				bat "(cd lib\\CMakeTools && git -c http.sslVerify=false reset --hard && git clean -fdx && git checkout main && git pull origin main)"
            }
        }

        stage('install Pegase'){
            steps{
                script {
                    //find the most recent install if not specified
                    if (params.installeur_pegase == '') {
                        pegase_exec = powershell (script: "python ${WORKSPACE}\\tests\\scripts\\get_last_pegase_version.py D:\\A_VERSION_PEGASE", returnStdout: true).trim()
                    } else {
                        pegase_exec = params.installeur_pegase
                    }
                //remove previous install if it exists
                bat "if exist ${WORKSPACE}\\lib\\PegaseInstall rmdir ${WORKSPACE}\\lib\\PegaseInstall /q /s"
                bat "D:\\A_VERSION_PEGASE\\${pegase_exec} -root ${WORKSPACE}\\lib\\PegaseInstall --al -c install PegaseLibs"
                }
            }
        }
			
        stage('build all'){
            steps{
                bat "call ${WORKSPACE}\\buildAll.bat fullrelease"
   
            }
        }

		stage('build wheel'){
            steps{
                bat "call ${WORKSPACE}\\buildWheel.bat CEA"
   
            }
        }
		
        stage('update version'){
            when {
                anyOf {
                    changeset "**"
                }
            }
            
            steps{
            script{
                //Incrémentation de version après un appel cron				
                if (!currentBuild.rawBuild.getCauses()[0].toString().contains('UserIdCause') && env.JENKINS_URL == "https://jenkins-lset.appli.gre-rke2.intra.cea.fr/" ){
                    bat "python tests\\scripts\\change_release.py"
                    bat "cd src && git commit -a -m \"update automatique de version\" && git push --set-upstream origin ${params.CAIRN_GIT_BRANCH} && git push"
                }
            }

            }

        }

        stage('tnr standalone'){
            steps{
                bat "echo 'testing'"
                bat "IF NOT EXIST ${WORKSPACE}\\tests\\reports (mkdir ${WORKSPACE}\\tests\\reports)" //reports folder
                bat "call ${WORKSPACE}\\tests\\startCtest.bat fullrelease"
				bat "call ${WORKSPACE}\\tests\\scripts\\cairnAPIinstall.bat ${WORKSPACE}\\bin"
                bat "call ${WORKSPACE}\\tests\\scripts\\startPytest.bat fullrelease Cairn ${WORKSPACE}\\tests\\reports\\Cairn-TNR.xml tests "//RUNCHECK YES" // 
                junit "tests/reports/*TNR.xml"
            }
        }
    }
    post {
      success {
          send_test_report()
          script{
            bat "echo 'testing start installer'"
            def installer_build = build job: 'Cairn_pipeline_installer', parameters: [string(name: 'EXPORT_PATH', value: "D:\\B_VERSION_CAIRN"), string(name : 'agent', value: "${params.agent}")]

            //if (installer_build.getResult() == "SUCCESS"){
            //    build job : "python_api_test", parameters: [string(name : 'agent', value: "${params.agent}")]
            //}

            }
      }
      unstable {
          send_test_report()
      }
      failure{
        script{
            //si un utilisateur lance le build il est le seul à recevoir un mail
            if ( currentBuild.rawBuild.getCauses()[0].toString().contains('UserIdCause') ){
                emailext (
                    recipientProviders: [requestor()],                
                    subject: "[NRT Cairn] Cairn pipeline",
                    body: "Un probleme est survenu lors des tests de Cairn : ${BUILD_URL} <br> Supprimer le dossier f:/pipeline_Persee_Project et relancer le pipeline peut resoudre le probleme",
                    mimeType: 'text/html'
                );
            //sinon si c'est un lancement automatique on fait avec la liste de base
            } else {
                emailext (
                    to: 'quentin.demarque@cea.fr,stephanie.crevon@cea.fr,pimprenelle.parmentier@cea.fr,ali.kassem@cea.fr,joel.chebassier@cea.fr,youcef.djebour@cea.fr,thibaut.wissocq@cea.fr,gabriele.leoncini@cea.fr',
                    subject: "[NRT Cairn] Cairn pipeline",
                    body: "Un probleme est survenu lors des tests de Cairn : ${BUILD_URL} <br> Supprimer le dossier f:/pipeline_Persee_Project et relancer le pipeline peut resoudre le probleme",
                    mimeType: 'text/html'
                );
            }
        }
      }
    }
}
//end pipeline

