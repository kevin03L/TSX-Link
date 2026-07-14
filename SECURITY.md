# Security Policy

## Supported Versions

Currently, we support the following versions with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.x     | :white_check_mark: |
| 0.x     | :x:                |

## Reporting a Vulnerability

Do not open public issues for security vulnerabilities. Instead, please email security@tsx-link.local with:

* Description of the vulnerability
* Steps to reproduce
* Potential impact
* Suggested fix (if any)

Please allow 90 days for a response before public disclosure.

## Security Best Practices

### Default Credentials

* Default admin password should be changed immediately after first boot
* No hardcoded credentials in firmware

### Network Security

* HTTPS enabled by default
* Self-signed certificates generated per device
* MQTT can be secured with TLS/SSL
* Modbus TCP should be used in trusted networks only

### Physical Security

* Device should be protected from physical tampering
* EEPROM contains sensitive configuration data
* Physical access to UART pins allows firmware read

### Firmware Updates

* All firmware updates should be cryptographically signed
* Verify checksum before installation
* Keep firmware up to date

### Device Hardening

1. Change default password immediately
2. Use strong Wi-Fi credentials
3. Disable AP mode in production
4. Use HTTPS for all communications
5. Monitor device logs regularly
6. Update firmware promptly
7. Use firewall rules to restrict access

## Known Limitations

* Modbus TCP has no built-in authentication - use firewall rules
* MQTT authentication is optional - enable in production
* REST API can be restricted using IP-based access control

## Security Contact

For security concerns, contact: security@tsx-link.local
