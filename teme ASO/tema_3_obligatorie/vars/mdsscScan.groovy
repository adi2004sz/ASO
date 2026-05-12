// vars/mdsscScan.groovy
// Custom Jenkins Step wrapping the OPSWAT MDSSC CLI Scanner Docker container

def call(Map config = [:]) {
    // Expected configuration parameters with defaults
    // `scanPath`: The directory or file path to scan relative to the Jenkins workspace root
    def scanPath = config.get('path', '.')
    
    // Credentials IDs stored in Jenkins Credentials Manager
    def serverCredId = config.get('serverCredId', 'mdssc-server-url')
    def apiKeyCredId = config.get('apiKeyCredId', 'mdssc-api-key')
    
    // MDSSC configuration
    def failOnVulnerabilities = config.get('failOnVulnerabilities', 'true')
    def threshold = config.get('threshold', 'high') // critical, high, medium, low

    echo "Running MDSSC scan on path: ${scanPath} with threshold: ${threshold}"

    // Assuming we manage credentials as "Secret Text" in Jenkins 
    // This securely injects the credentials into the environment variables without leaking them
    withCredentials([
        string(credentialsId: serverCredId, variable: 'MDSSC_SERVER'),
        string(credentialsId: apiKeyCredId, variable: 'MDSSC_API_KEY')
    ]) {
        // We use \$ to indicate shell environment variables rather than Groovy string interpolation
        // \$(pwd) maps the current Jenkins node's execution context properly
        sh """
        docker run --rm \\
          -e MDSSC_SERVER="\$MDSSC_SERVER" \\
          -e MDSSC_API_KEY="\$MDSSC_API_KEY" \\
          -e FAIL_ON_VULNERABILITIES="${failOnVulnerabilities}" \\
          -e VULNERABILITY_THRESHOLD="${threshold}" \\
          -v "\$(pwd)/${scanPath}:/scan" \\
          opswat/mdssc-scanner:latest \\
          /scan
        """
    }
}
