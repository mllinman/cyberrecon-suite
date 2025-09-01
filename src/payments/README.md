# Stripe Payment Integration

## Setup Instructions

1. **Get Your Stripe Keys**
   - Sign up at https://dashboard.stripe.com
   - Go to Developers > API Keys
   - Copy your Secret Key (starts with `sk_test_` for testing)

2. **Configure API Key**
   - Open `src/payments/PaymentDialog.cpp`
   - Replace `"sk_test_your_stripe_secret_key_here"` with your actual key
   - Line 23: `stripeManager->setApiKey("sk_test_your_actual_key_here");`

3. **Create Products in Stripe Dashboard**
   - Go to Products in your Stripe dashboard
   - Create "Professional Plan" product with $99/month price
   - Create "Enterprise Plan" product with $299/month price
   - Copy the Price IDs (start with `price_`)

4. **Update Price IDs**
   - In `src/profile/UserProfile.cpp` line 15: Replace `"price_enterprise_monthly"`
   - In `src/auth/LoginWindow.cpp` lines for Professional and Enterprise plans

## Features Included

✅ **Complete Stripe Integration**
- Customer creation and management
- Subscription creation and cancellation
- Payment method handling
- Error handling and validation

✅ **Professional Payment UI**
- Card number formatting (spaces every 4 digits)
- Expiry date formatting (MM/YY)
- Real-time validation
- Progress indicators

✅ **Subscription Management**
- Upgrade from login screen
- Upgrade from user profile
- Database integration
- Status updates

✅ **Security Features**
- Stripe-hosted payment processing
- No card data stored locally
- Secure API communication
- Error handling

## Testing

Use Stripe's test card numbers:
- **Success**: 4242 4242 4242 4242
- **Declined**: 4000 0000 0000 0002
- **Requires Authentication**: 4000 0025 0000 3155

Any future expiry date and any 3-digit CVC will work for testing.

## Production Deployment

1. Replace test API keys with live keys
2. Update webhook endpoints if needed
3. Test thoroughly in Stripe's test mode first
4. Enable live mode in Stripe dashboard